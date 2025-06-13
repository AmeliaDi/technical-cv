#!/usr/bin/env python3
"""
ARM LLM Runtime Python Example

This example demonstrates how to use the ARM LLM Runtime for various LLM inference tasks.
"""

import sys
import time
import threading
from typing import List, Optional

try:
    import arm_llm_runtime as llm
except ImportError:
    print("Error: arm_llm_runtime module not found. Please build and install the Python bindings.")
    sys.exit(1)

def basic_generation_example():
    """Basic text generation example"""
    print("=== Basic Generation Example ===")
    
    # Load a model with default settings
    print("Loading DialoGPT-medium model...")
    model = llm.load_model("microsoft/DialoGPT-medium", quantize="Q4_K")
    
    # Generate response to a simple prompt
    prompt = "Hello, how are you today?"
    print(f"Prompt: {prompt}")
    
    response = model.generate(prompt)
    print(f"Response: {response.text}")
    print(f"Stats: {response.tokens_generated} tokens in {response.generation_time:.2f}s "
          f"({response.tokens_per_second:.1f} tokens/s)")
    print()

def advanced_generation_example():
    """Advanced generation with custom configuration"""
    print("=== Advanced Generation Example ===")
    
    # Create runtime with custom configuration
    config = llm.RuntimeConfig()
    config.memory_pool_size = 4 * 1024 * 1024 * 1024  # 4GB
    config.num_threads = 4
    config.cache_dir = "./models"
    
    runtime = llm.ArmLLMRuntime(config)
    
    # Load model with specific quantization
    print("Loading Llama-2-7b-chat model with Q4_K quantization...")
    model = runtime.load_model("meta-llama/Llama-2-7b-chat-hf", llm.QuantizationType.Q4_K)
    
    # Configure generation parameters
    gen_config = llm.GenerationConfig()
    gen_config.max_tokens = 150
    gen_config.temperature = 0.8
    gen_config.top_k = 40
    gen_config.top_p = 0.9
    gen_config.repetition_penalty = 1.1
    gen_config.stop_sequences = ["Human:", "Assistant:"]
    
    # Generate with custom configuration
    prompt = "Explain the concept of artificial intelligence in simple terms."
    print(f"Prompt: {prompt}")
    
    result = model.generate(prompt, gen_config)
    print(f"Response: {result.text}")
    print(f"Performance: {result.tokens_per_second:.1f} tokens/s, "
          f"Memory used: {llm.utils.format_bytes(result.memory_used)}")
    print()

def streaming_generation_example():
    """Streaming generation example"""
    print("=== Streaming Generation Example ===")
    
    model = llm.load_model("microsoft/DialoGPT-medium")
    
    print("Generating story with streaming output...")
    print("Output: ", end="", flush=True)
    
    def on_token(token: str):
        print(token, end="", flush=True)
    
    gen_config = llm.GenerationConfig()
    gen_config.max_tokens = 100
    gen_config.temperature = 0.9
    
    model.generate_stream("Tell me a short story about a robot", on_token, gen_config)
    print("\n")

def batch_generation_example():
    """Batch generation example"""
    print("=== Batch Generation Example ===")
    
    model = llm.load_model("microsoft/DialoGPT-medium")
    
    prompts = [
        "What is machine learning?",
        "How do neural networks work?",
        "Explain deep learning in simple terms.",
        "What are the applications of AI?"
    ]
    
    print("Generating responses for multiple prompts...")
    gen_config = llm.GenerationConfig()
    gen_config.max_tokens = 80
    
    start_time = time.time()
    results = model.generate_batch(prompts, gen_config)
    end_time = time.time()
    
    for i, (prompt, result) in enumerate(zip(prompts, results)):
        print(f"\nPrompt {i+1}: {prompt}")
        print(f"Response: {result.text}")
    
    total_tokens = sum(r.tokens_generated for r in results)
    total_time = end_time - start_time
    print(f"\nBatch performance: {total_tokens} tokens in {total_time:.2f}s "
          f"({total_tokens/total_time:.1f} tokens/s)")
    print()

def model_comparison_example():
    """Compare different models and quantization levels"""
    print("=== Model Comparison Example ===")
    
    runtime = llm.ArmLLMRuntime()
    
    models_to_test = [
        ("microsoft/DialoGPT-medium", llm.QuantizationType.Q4_K),
        ("microsoft/DialoGPT-medium", llm.QuantizationType.Q8_0),
    ]
    
    prompt = "What is the meaning of life?"
    
    for model_id, quantization in models_to_test:
        print(f"\nTesting {model_id} with {quantization}...")
        
        # Load model
        load_start = time.time()
        model = runtime.load_model(model_id, quantization)
        load_time = time.time() - load_start
        
        # Get model info
        info = model.info()
        print(f"Model info: {info.vocab_size} vocab, {info.num_layers} layers")
        print(f"Load time: {load_time:.2f}s")
        
        # Generate response
        result = model.generate(prompt)
        print(f"Response: {result.text[:100]}...")
        print(f"Speed: {result.tokens_per_second:.1f} tokens/s")
        print(f"Memory: {llm.utils.format_bytes(result.memory_used)}")

def interactive_chat_example():
    """Interactive chat example"""
    print("=== Interactive Chat Example ===")
    print("Starting interactive chat session...")
    print("Type 'quit' to exit, 'help' for commands")
    
    model = llm.load_model("microsoft/DialoGPT-medium")
    
    gen_config = llm.GenerationConfig()
    gen_config.max_tokens = 100
    gen_config.temperature = 0.7
    
    conversation_history = []
    
    while True:
        try:
            user_input = input("\nYou: ").strip()
            
            if user_input.lower() in ['quit', 'exit', 'bye']:
                print("Goodbye!")
                break
            
            if user_input.lower() == 'help':
                print("Commands:")
                print("  quit/exit/bye - Exit chat")
                print("  help         - Show this help")
                print("  clear        - Clear conversation history")
                print("  info         - Show model information")
                continue
            
            if user_input.lower() == 'clear':
                conversation_history.clear()
                print("Conversation history cleared.")
                continue
            
            if user_input.lower() == 'info':
                info = model.info()
                print(f"Model: {info.name}")
                print(f"Type: {info.type}")
                print(f"Quantization: {info.quantization}")
                continue
            
            if not user_input:
                continue
            
            # Add user input to conversation
            conversation_history.append(f"Human: {user_input}")
            
            # Create prompt with conversation context
            context = "\n".join(conversation_history[-5:])  # Keep last 5 exchanges
            prompt = context + "\nAssistant:"
            
            # Generate response
            result = model.generate(prompt, gen_config)
            
            # Extract just the assistant's response
            response = result.text.split("Assistant:")[-1].strip()
            conversation_history.append(f"Assistant: {response}")
            
            print(f"Assistant: {response}")
            
        except KeyboardInterrupt:
            print("\nExiting...")
            break
        except Exception as e:
            print(f"Error: {e}")

def performance_monitoring_example():
    """Performance monitoring example"""
    print("=== Performance Monitoring Example ===")
    
    runtime = llm.ArmLLMRuntime()
    model = runtime.load_model("microsoft/DialoGPT-medium")
    
    # Reset metrics
    runtime.reset_metrics()
    
    # Generate several responses
    prompts = [
        "What is AI?",
        "How does machine learning work?",
        "Explain neural networks.",
        "What are the benefits of automation?",
        "How can AI help in healthcare?"
    ]
    
    print("Generating multiple responses to collect metrics...")
    for prompt in prompts:
        result = model.generate(prompt)
        print(f"Generated {result.tokens_generated} tokens")
    
    # Get performance metrics
    metrics = runtime.get_metrics()
    
    print(f"\nPerformance Metrics:")
    print(f"Total inference time: {metrics.total_inference_time:.2f}s")
    print(f"Average tokens/sec: {metrics.avg_tokens_per_second:.1f}")
    print(f"Total tokens generated: {metrics.total_tokens_generated}")
    print(f"Memory used: {llm.utils.format_bytes(metrics.memory_used)}")
    print(f"Cache hits: {metrics.cache_hits}")
    print(f"Cache misses: {metrics.cache_misses}")
    
    # System information
    print(f"\nSystem Information:")
    print(llm.utils.get_system_info())
    print()

def quantization_example():
    """Model quantization example"""
    print("=== Model Quantization Example ===")
    
    runtime = llm.ArmLLMRuntime()
    
    # Example of quantizing a model (in practice, you'd have the original model file)
    print("Example quantization configuration:")
    
    quant_config = llm.QuantizationConfig()
    quant_config.method = llm.QuantizationType.Q4_K
    quant_config.bits = 4
    quant_config.group_size = 128
    quant_config.symmetric = False
    
    print(f"Quantization method: {quant_config.method}")
    print(f"Bits: {quant_config.bits}")
    print(f"Group size: {quant_config.group_size}")
    print(f"Symmetric: {quant_config.symmetric}")
    
    # Note: Actual quantization would require original model files
    print("Note: Actual quantization requires original model files")
    print()

def main():
    """Main function to run all examples"""
    print("ARM LLM Runtime Python Examples")
    print("=" * 40)
    
    # Check system info
    print("System Information:")
    print(llm.utils.get_system_info())
    print(f"NEON support: {llm.utils.has_neon_support()}")
    print()
    
    examples = [
        ("Basic Generation", basic_generation_example),
        ("Advanced Generation", advanced_generation_example),
        ("Streaming Generation", streaming_generation_example),
        ("Batch Generation", batch_generation_example),
        ("Model Comparison", model_comparison_example),
        ("Performance Monitoring", performance_monitoring_example),
        ("Quantization Config", quantization_example),
    ]
    
    if len(sys.argv) > 1:
        if sys.argv[1] == "interactive":
            interactive_chat_example()
            return
        elif sys.argv[1] == "help":
            print("Available examples:")
            for i, (name, _) in enumerate(examples):
                print(f"  {i+1}. {name}")
            print("\nUsage:")
            print(f"  {sys.argv[0]}           # Run all examples")
            print(f"  {sys.argv[0]} interactive # Run interactive chat")
            print(f"  {sys.argv[0]} <number>    # Run specific example")
            return
        else:
            try:
                example_num = int(sys.argv[1]) - 1
                if 0 <= example_num < len(examples):
                    name, func = examples[example_num]
                    print(f"Running example: {name}")
                    func()
                    return
                else:
                    print(f"Invalid example number. Choose 1-{len(examples)}")
                    return
            except ValueError:
                print("Invalid argument. Use 'help' for usage.")
                return
    
    # Run all examples
    for name, func in examples:
        try:
            func()
        except Exception as e:
            print(f"Error in {name}: {e}")
        print("-" * 40)

if __name__ == "__main__":
    main() 