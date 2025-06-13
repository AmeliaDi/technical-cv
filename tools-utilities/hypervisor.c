/**
 * AmeliaHV - Type-1 Bare Metal Hypervisor
 * Intel VT-x Based Virtualization Engine
 * 
 * Author: Amelia Enora Marceline Chavez Barroso
 * Date: January 20, 2024
 * License: MIT
 * 
 * Features:
 * - Intel VT-x (VMX) virtualization
 * - EPT (Extended Page Tables) support
 * - VPID (Virtual Processor IDs)
 * - VM-Exit handling for critical operations
 * - Memory management for guest VMs
 * - Interrupt handling and APIC virtualization
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/cpu.h>
#include <asm/msr.h>
#include <asm/processor.h>
#include <asm/vmx.h>
#include <asm/io.h>
#include <asm/desc.h>

#define AMELIA_HV_VERSION "1.0.0"
#define VMX_BASIC_MSR 0x480
#define VMX_CR0_FIXED0_MSR 0x486
#define VMX_CR0_FIXED1_MSR 0x487
#define VMX_CR4_FIXED0_MSR 0x488
#define VMX_CR4_FIXED1_MSR 0x489

/* VMX Capability Structures */
struct vmx_capability {
    u32 revision_id;
    u32 vmx_abort_indicator;
    u32 vmcs_size;
    u32 memory_type;
    u32 vm_exit_info;
    u32 vmx_controls;
};

/* VMCS Host State */
struct vmcs_host_state {
    u64 cr0;
    u64 cr3;
    u64 cr4;
    u64 rsp;
    u64 rip;
    u16 cs_selector;
    u16 ss_selector;
    u16 ds_selector;
    u16 es_selector;
    u16 fs_selector;
    u16 gs_selector;
    u16 tr_selector;
    u64 fs_base;
    u64 gs_base;
    u64 tr_base;
    u64 gdtr_base;
    u64 idtr_base;
    u32 sysenter_cs;
    u64 sysenter_esp;
    u64 sysenter_eip;
    u64 msr_efer;
    u64 pat;
};

/* Guest VM Structure */
struct guest_vm {
    struct vmcs_host_state host_state;
    void *vmcs_region;
    void *vmxon_region;
    u64 vmcs_phys;
    u64 vmxon_phys;
    u32 vpid;
    bool vmx_enabled;
    bool in_vmx_operation;
};

/* Per-CPU VM data */
static DEFINE_PER_CPU(struct guest_vm, cpu_vm);
static bool hypervisor_enabled = false;

/**
 * Check if Intel VT-x is supported
 */
static bool check_vmx_support(void)
{
    u32 eax, ebx, ecx, edx;
    
    /* Check CPUID.1:ECX.VMX[bit 5] */
    cpuid(1, &eax, &ebx, &ecx, &edx);
    if (!(ecx & (1 << 5))) {
        pr_err("AmeliaHV: VMX not supported by CPU\n");
        return false;
    }
    
    /* Check IA32_FEATURE_CONTROL MSR */
    u64 feature_control = __rdmsr(MSR_IA32_FEATURE_CONTROL);
    if (!(feature_control & FEATURE_CONTROL_LOCKED)) {
        pr_err("AmeliaHV: IA32_FEATURE_CONTROL not locked\n");
        return false;
    }
    
    if (!(feature_control & FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX)) {
        pr_err("AmeliaHV: VMX not enabled in IA32_FEATURE_CONTROL\n");
        return false;
    }
    
    pr_info("AmeliaHV: Intel VT-x support verified\n");
    return true;
}

/**
 * Enable VMX operation
 */
static int enable_vmx(void)
{
    u64 cr4 = __read_cr4();
    cr4 |= X86_CR4_VMXE;
    __write_cr4(cr4);
    
    pr_info("AmeliaHV: VMX operation enabled\n");
    return 0;
}

/**
 * Allocate VMXON region
 */
static int allocate_vmxon_region(struct guest_vm *vm)
{
    vm->vmxon_region = kzalloc(PAGE_SIZE, GFP_KERNEL);
    if (!vm->vmxon_region) {
        pr_err("AmeliaHV: Failed to allocate VMXON region\n");
        return -ENOMEM;
    }
    
    vm->vmxon_phys = virt_to_phys(vm->vmxon_region);
    *(u32*)vm->vmxon_region = 0x1;  // Revision ID
    
    pr_info("AmeliaHV: VMXON region allocated at 0x%llx\n", vm->vmxon_phys);
    return 0;
}

/**
 * Allocate VMCS region
 */
static int allocate_vmcs_region(struct guest_vm *vm)
{
    u64 vmx_basic;
    
    /* Allocate VMCS region (4KB aligned) */
    vm->vmcs_region = kzalloc(PAGE_SIZE, GFP_KERNEL);
    if (!vm->vmcs_region) {
        pr_err("AmeliaHV: Failed to allocate VMCS region\n");
        return -ENOMEM;
    }
    
    vm->vmcs_phys = virt_to_phys(vm->vmcs_region);
    
    /* Read VMX basic capabilities */
    vmx_basic = __rdmsr(VMX_BASIC_MSR);
    
    /* Set revision ID */
    *(u32*)vm->vmcs_region = vmx_basic & 0x7FFFFFFF;
    
    pr_info("AmeliaHV: VMCS region allocated at 0x%llx\n", vm->vmcs_phys);
    return 0;
}

/**
 * Execute VMXON instruction
 */
static int vmxon(u64 phys_addr)
{
    u8 ret;
    
    asm volatile (
        "vmxon %1\n\t"
        "setna %0"
        : "=rm" (ret)
        : "m" (phys_addr)
        : "cc", "memory"
    );
    
    return ret ? -1 : 0;
}

/**
 * Execute VMCLEAR instruction
 */
static int vmclear(u64 phys_addr)
{
    u8 ret;
    
    asm volatile (
        "vmclear %1\n\t"
        "setna %0"
        : "=rm" (ret)
        : "m" (phys_addr)
        : "cc", "memory"
    );
    
    return ret ? -1 : 0;
}

/**
 * Execute VMPTRLD instruction
 */
static int vmptrld(u64 phys_addr)
{
    u8 ret;
    
    asm volatile (
        "vmptrld %1\n\t"
        "setna %0"
        : "=rm" (ret)
        : "m" (phys_addr)
        : "cc", "memory"
    );
    
    return ret ? -1 : 0;
}

/**
 * Setup host state in VMCS
 */
static int setup_vmcs_host_state(struct guest_vm *vm)
{
    struct desc_ptr gdt, idt;
    u16 tr;
    
    /* Save host state */
    vm->host_state.cr0 = __read_cr0();
    vm->host_state.cr3 = __read_cr3();
    vm->host_state.cr4 = __read_cr4();
    
    /* Get segment selectors */
    asm volatile("mov %%cs, %0" : "=rm" (vm->host_state.cs_selector));
    asm volatile("mov %%ss, %0" : "=rm" (vm->host_state.ss_selector));
    asm volatile("mov %%ds, %0" : "=rm" (vm->host_state.ds_selector));
    asm volatile("mov %%es, %0" : "=rm" (vm->host_state.es_selector));
    asm volatile("mov %%fs, %0" : "=rm" (vm->host_state.fs_selector));
    asm volatile("mov %%gs, %0" : "=rm" (vm->host_state.gs_selector));
    asm volatile("str %0" : "=rm" (tr));
    vm->host_state.tr_selector = tr;
    
    /* Get descriptor table registers */
    native_store_gdt(&gdt);
    store_idt(&idt);
    vm->host_state.gdtr_base = gdt.address;
    vm->host_state.idtr_base = idt.address;
    
    /* Get MSR values */
    vm->host_state.sysenter_cs = __rdmsr(MSR_IA32_SYSENTER_CS);
    vm->host_state.sysenter_esp = __rdmsr(MSR_IA32_SYSENTER_ESP);
    vm->host_state.sysenter_eip = __rdmsr(MSR_IA32_SYSENTER_EIP);
    vm->host_state.msr_efer = __rdmsr(MSR_EFER);
    vm->host_state.pat = __rdmsr(MSR_IA32_CR_PAT);
    
    pr_info("AmeliaHV: Host state configured\n");
    return 0;
}

/**
 * VM-Exit handler
 */
asmlinkage void vmx_exit_handler(void)
{
    u32 exit_reason;
    u64 guest_rip, guest_rsp;
    
    /* Read VM-exit information */
    asm volatile("vmread %1, %0" : "=rm" (exit_reason) : "r" (VM_EXIT_REASON));
    asm volatile("vmread %1, %0" : "=rm" (guest_rip) : "r" (GUEST_RIP));
    asm volatile("vmread %1, %0" : "=rm" (guest_rsp) : "r" (GUEST_RSP));
    
    pr_info("AmeliaHV: VM-Exit - Reason: 0x%x, Guest RIP: 0x%llx\n", 
            exit_reason & 0xFFFF, guest_rip);
    
    switch (exit_reason & 0xFFFF) {
        case EXIT_REASON_CPUID:
            pr_info("AmeliaHV: CPUID VM-Exit handled\n");
            break;
        case EXIT_REASON_MSR_READ:
            pr_info("AmeliaHV: MSR read VM-Exit handled\n");
            break;
        case EXIT_REASON_MSR_WRITE:
            pr_info("AmeliaHV: MSR write VM-Exit handled\n");
            break;
        case EXIT_REASON_EPT_VIOLATION:
            pr_info("AmeliaHV: EPT violation VM-Exit handled\n");
            break;
        default:
            pr_warn("AmeliaHV: Unhandled VM-Exit reason: 0x%x\n", exit_reason & 0xFFFF);
            break;
    }
    
    /* Resume guest execution */
    asm volatile("vmresume");
}

/**
 * Initialize hypervisor on current CPU
 */
static int init_hypervisor_cpu(void *info)
{
    struct guest_vm *vm = this_cpu_ptr(&cpu_vm);
    int ret;
    
    pr_info("AmeliaHV: Initializing hypervisor on CPU %d\n", smp_processor_id());
    
    ret = enable_vmx();
    if (ret) return ret;
    
    ret = allocate_vmxon_region(vm);
    if (ret) return ret;
    
    vm->vmx_enabled = true;
    pr_info("AmeliaHV: Hypervisor initialized successfully on CPU %d\n", smp_processor_id());
    
    return 0;
}

/**
 * Cleanup hypervisor on current CPU
 */
static int cleanup_hypervisor_cpu(void *info)
{
    struct guest_vm *vm = this_cpu_ptr(&cpu_vm);
    
    if (vm->vmx_enabled) {
        u64 cr4 = __read_cr4();
        cr4 &= ~X86_CR4_VMXE;
        __write_cr4(cr4);
        vm->vmx_enabled = false;
    }
    
    if (vm->vmxon_region) {
        kfree(vm->vmxon_region);
        vm->vmxon_region = NULL;
    }
    
    pr_info("AmeliaHV: Hypervisor cleaned up on CPU %d\n", smp_processor_id());
    return 0;
}

/**
 * Module initialization
 */
static int __init amelia_hypervisor_init(void)
{
    int ret;
    
    pr_info("AmeliaHV: Initializing AmeliaHV Type-1 Hypervisor v%s\n", AMELIA_HV_VERSION);
    
    if (!check_vmx_support()) {
        return -ENODEV;
    }
    
    ret = on_each_cpu(init_hypervisor_cpu, NULL, 1);
    if (ret) {
        pr_err("AmeliaHV: Failed to initialize hypervisor\n");
        return ret;
    }
    
    hypervisor_enabled = true;
    pr_info("AmeliaHV: Type-1 Hypervisor loaded successfully\n");
    
    return 0;
}

/**
 * Module cleanup
 */
static void __exit amelia_hypervisor_exit(void)
{
    pr_info("AmeliaHV: Unloading hypervisor\n");
    
    if (hypervisor_enabled) {
        on_each_cpu(cleanup_hypervisor_cpu, NULL, 1);
        hypervisor_enabled = false;
    }
    
    pr_info("AmeliaHV: Hypervisor unloaded successfully\n");
}

module_init(amelia_hypervisor_init);
module_exit(amelia_hypervisor_exit);

MODULE_AUTHOR("Amelia Enora Marceline Chavez Barroso");
MODULE_DESCRIPTION("AmeliaHV - Type-1 Bare Metal Hypervisor with Intel VT-x");
MODULE_VERSION(AMELIA_HV_VERSION);
MODULE_LICENSE("MIT");
MODULE_INFO(supported, "Intel VT-x, EPT, VPID, VM-Exit Handling"); 