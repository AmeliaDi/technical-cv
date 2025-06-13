#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <iomanip>
#include "../src/arm_llm_runtime.h"

using namespace arm_llm;

void print_separator(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void basic_generation_example() {
    print_separator("Basic Generation Example");
    
    try {
        // Create runtime with default configuration
        ArmLLMRuntime runtime;
        
        // Load a model from HuggingFace
        std::cout << "Loading DialoGPT-medium model..." << std::endl;
        auto model = runtime.loadModel("microsoft/DialoGPT-medium", QuantizationType::Q4_K);
        
        // Generate response
        std::string prompt = "Hello, how are you today?";
        std::cout << "Prompt: " << prompt << std::endl;
        
        GenerationConfig config;
        config.max_tokens = 50;
        config.temperature = 0.7f;
        
        auto result = model->generate(prompt, config);
        
        std::cout << "Response: " << result.text << std::endl;
        std::cout << "Generated " << result.tokens_generated << " tokens in " 
                  << std::fixed << std::setprecision(2) << result.generation_time 
                  << "s (" << std::setprecision(1) << result.tokens_per_second 
                  << " tokens/s)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void advanced_configuration_example() {
    print_separator("Advanced Configuration Example");
    
    try {
        // Create custom runtime configuration
        RuntimeConfig config;
        config.memory_pool_size = 4ULL * 1024 * 1024 * 1024; // 4GB
        config.num_threads = 4;
        config.cache_dir = "./custom_models";
        config.use_neon = true;
        config.use_flash_attention = true;
        
        ArmLLMRuntime runtime(config);
        
        std::cout << "Runtime configured with:" << std::endl;
        std::cout << "  Memory pool: " << utils::format_bytes(config.memory_pool_size) << std::endl;
        std::cout << "  Threads: " << config.num_threads << std::endl;
        std::cout << "  NEON enabled: " << (config.use_neon ? "Yes" : "No") << std::endl;
        std::cout << "  Flash Attention: " << (config.use_flash_attention ? "Yes" : "No") << std::endl;
        
        // Load model with specific quantization
        auto model = runtime.loadModel("microsoft/DialoGPT-medium", QuantizationType::Q8_0);
        
        // Custom generation configuration
        GenerationConfig gen_config;
        gen_config.max_tokens = 100;
        gen_config.temperature = 0.8f;
        gen_config.top_k = 40;
        gen_config.top_p = 0.9f;
        gen_config.repetition_penalty = 1.1f;
        gen_config.stop_sequences = {"Human:", "Assistant:"};
        
        std::string prompt = "Explain artificial intelligence in simple terms.";
        auto result = model->generate(prompt, gen_config);
        
        std::cout << "\nGenerated response:" << std::endl;
        std::cout << result.text << std::endl;
        std::cout << "\nPerformance: " << result.tokens_per_second << " tokens/s" << std::endl;
        std::cout << "Memory used: " << utils::format_bytes(result.memory_used) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void streaming_generation_example() {
    print_separator("Streaming Generation Example");
    
    try {
        ArmLLMRuntime runtime;
        auto model = runtime.loadModel("microsoft/DialoGPT-medium");
        
        std::cout << "Generating story with streaming output..." << std::endl;
        std::cout << "Story: ";
        std::cout.flush();
        
        GenerationConfig config;
        config.max_tokens = 80;
        config.temperature = 0.9f;
        
        model->generateStream("Tell me a short story about a robot", 
                             [](const std::string& token) {
                                 std::cout << token << std::flush;
                             }, config);
        
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void batch_generation_example() {
    print_separator("Batch Generation Example");
    
    try {
        ArmLLMRuntime runtime;
        auto model = runtime.loadModel("microsoft/DialoGPT-medium");
        
        std::vector<std::string> prompts = {
            "What is machine learning?",
            "How do neural networks work?",
            "Explain deep learning briefly.",
            "What are the applications of AI?"
        };
        
        std::cout << "Generating responses for " << prompts.size() << " prompts..." << std::endl;
        
        GenerationConfig config;
        config.max_tokens = 60;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        auto results = model->generateBatch(prompts, config);
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        for (size_t i = 0; i < prompts.size(); ++i) {
            std::cout << "\nPrompt " << (i + 1) << ": " << prompts[i] << std::endl;
            std::cout << "Response: " << results[i].text << std::endl;
        }
        
        int total_tokens = 0;
        for (const auto& result : results) {
            total_tokens += result.tokens_generated;
        }
        
        std::cout << "\nBatch performance:" << std::endl;
        std::cout << "Total tokens: " << total_tokens << std::endl;
        std::cout << "Total time: " << total_time.count() / 1000.0 << "s" << std::endl;
        std::cout << "Average speed: " << (total_tokens * 1000.0) / total_time.count() << " tokens/s" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void model_comparison_example() {
    print_separator("Model Comparison Example");
    
    try {
        ArmLLMRuntime runtime;
        
        std::vector<std::pair<std::string, QuantizationType>> models_to_test = {
            {"microsoft/DialoGPT-medium", QuantizationType::Q4_K},
            {"microsoft/DialoGPT-medium", QuantizationType::Q8_0}
        };
        
        std::string prompt = "What is the meaning of life?";
        
        for (const auto& [model_id, quantization] : models_to_test) {
            std::cout << "\nTesting " << model_id << " with quantization " 
                      << static_cast<int>(quantization) << std::endl;
            
            // Measure model loading time
            auto load_start = std::chrono::high_resolution_clock::now();
            auto model = runtime.loadModel(model_id, quantization);
            auto load_end = std::chrono::high_resolution_clock::now();
            
            auto load_time = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start);
            
            // Get model information
            const auto& info = model->info();
            std::cout << "Model info: " << info.vocab_size << " vocab, " 
                      << info.num_layers << " layers, " 
                      << info.context_length << " context" << std::endl;
            std::cout << "Load time: " << load_time.count() << "ms" << std::endl;
            
            // Generate response
            auto result = model->generate(prompt);
            std::cout << "Response: " << result.text.substr(0, 100) << "..." << std::endl;
            std::cout << "Speed: " << result.tokens_per_second << " tokens/s" << std::endl;
            std::cout << "Memory: " << utils::format_bytes(result.memory_used) << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void performance_monitoring_example() {
    print_separator("Performance Monitoring Example");
    
    try {
        ArmLLMRuntime runtime;
        auto model = runtime.loadModel("microsoft/DialoGPT-medium");
        
        // Reset metrics
        runtime.resetMetrics();
        
        std::vector<std::string> test_prompts = {
            "What is AI?",
            "How does machine learning work?",
            "Explain neural networks.",
            "What are the benefits of automation?",
            "How can AI help in healthcare?"
        };
        
        std::cout << "Running performance tests..." << std::endl;
        
        for (const auto& prompt : test_prompts) {
            auto result = model->generate(prompt);
            std::cout << "Generated " << result.tokens_generated << " tokens in " 
                      << std::fixed << std::setprecision(2) << result.generation_time << "s" << std::endl;
        }
        
        // Get comprehensive metrics
        auto metrics = runtime.getMetrics();
        
        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  Total inference time: " << std::fixed << std::setprecision(2) 
                  << metrics.total_inference_time << "s" << std::endl;
        std::cout << "  Average tokens/sec: " << std::setprecision(1) 
                  << metrics.avg_tokens_per_second << std::endl;
        std::cout << "  Total tokens generated: " << metrics.total_tokens_generated << std::endl;
        std::cout << "  Memory used: " << utils::format_bytes(metrics.memory_used) << std::endl;
        std::cout << "  Cache hits: " << metrics.cache_hits << std::endl;
        std::cout << "  Cache misses: " << metrics.cache_misses << std::endl;
        
        // System information
        std::cout << "\nSystem Information:" << std::endl;
        std::cout << utils::get_system_info() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void tensor_operations_example() {
    print_separator("Tensor Operations Example");
    
    try {
        // Create tensors
        Tensor tensor1({1000, 1000}, DataType::FLOAT32);
        Tensor tensor2({1000, 1000}, DataType::FLOAT32);
        Tensor result({1000, 1000}, DataType::FLOAT32);
        
        std::cout << "Created tensors of shape [1000, 1000]" << std::endl;
        std::cout << "Tensor size: " << utils::format_bytes(tensor1.bytes()) << " each" << std::endl;
        
        // Fill tensors with test data
        tensor1.fill(1.0f);
        tensor2.fill(2.0f);
        
        std::cout << "Filled tensors with test data" << std::endl;
        
        // Perform operations (simplified example)
        tensor1.add(tensor2);
        std::cout << "Performed tensor addition" << std::endl;
        
        // Zero out result tensor
        result.zero();
        std::cout << "Zeroed result tensor" << std::endl;
        
        std::cout << "Tensor operations completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void quantization_example() {
    print_separator("Quantization Example");
    
    try {
        ArmLLMRuntime runtime;
        
        // Example quantization configuration
        QuantizationConfig config;
        config.method = QuantizationType::Q4_K;
        config.bits = 4;
        config.group_size = 128;
        config.symmetric = false;
        config.scale_factor = 1.0;
        
        std::cout << "Quantization Configuration:" << std::endl;
        std::cout << "  Method: " << static_cast<int>(config.method) << std::endl;
        std::cout << "  Bits: " << config.bits << std::endl;
        std::cout << "  Group size: " << config.group_size << std::endl;
        std::cout << "  Symmetric: " << (config.symmetric ? "Yes" : "No") << std::endl;
        std::cout << "  Scale factor: " << config.scale_factor << std::endl;
        
        // Note: Actual quantization would require model files
        std::cout << "\nNote: Actual model quantization requires original model files" << std::endl;
        std::cout << "This example shows configuration only" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void interactive_chat_example() {
    print_separator("Interactive Chat Example");
    
    try {
        ArmLLMRuntime runtime;
        auto model = runtime.loadModel("microsoft/DialoGPT-medium");
        
        std::cout << "Starting interactive chat session..." << std::endl;
        std::cout << "Type 'quit' to exit, 'help' for commands" << std::endl;
        
        GenerationConfig config;
        config.max_tokens = 80;
        config.temperature = 0.7f;
        
        std::vector<std::string> conversation_history;
        std::string input;
        
        while (true) {
            std::cout << "\nYou: ";
            std::getline(std::cin, input);
            
            if (input.empty()) continue;
            
            if (input == "quit" || input == "exit") {
                std::cout << "Goodbye!" << std::endl;
                break;
            }
            
            if (input == "help") {
                std::cout << "Commands:" << std::endl;
                std::cout << "  quit/exit - Exit chat" << std::endl;
                std::cout << "  help      - Show this help" << std::endl;
                std::cout << "  clear     - Clear conversation history" << std::endl;
                std::cout << "  stats     - Show performance statistics" << std::endl;
                continue;
            }
            
            if (input == "clear") {
                conversation_history.clear();
                std::cout << "Conversation history cleared." << std::endl;
                continue;
            }
            
            if (input == "stats") {
                auto metrics = runtime.getMetrics();
                std::cout << "Performance Statistics:" << std::endl;
                std::cout << "  Avg tokens/sec: " << metrics.avg_tokens_per_second << std::endl;
                std::cout << "  Total tokens: " << metrics.total_tokens_generated << std::endl;
                std::cout << "  Memory used: " << utils::format_bytes(metrics.memory_used) << std::endl;
                continue;
            }
            
            // Add to conversation history
            conversation_history.push_back("Human: " + input);
            
            // Keep only last 10 exchanges
            if (conversation_history.size() > 10) {
                conversation_history.erase(conversation_history.begin(), 
                                         conversation_history.begin() + (conversation_history.size() - 10));
            }
            
            // Create context
            std::string context;
            for (const auto& line : conversation_history) {
                context += line + "\n";
            }
            context += "Assistant:";
            
            // Generate response
            auto result = model->generate(context, config);
            
            // Extract just the assistant's response
            std::string response = result.text;
            size_t pos = response.find("Assistant:");
            if (pos != std::string::npos) {
                response = response.substr(pos + 10);
            }
            
            conversation_history.push_back("Assistant: " + response);
            
            std::cout << "Assistant: " << response << std::endl;
            std::cout << "[" << std::fixed << std::setprecision(1) 
                      << result.tokens_per_second << " tokens/s]" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "ARM LLM Runtime C++ Examples" << std::endl;
    std::cout << "============================" << std::endl;
    
    // Show system information
    std::cout << "System Information:" << std::endl;
    std::cout << utils::get_system_info() << std::endl;
    std::cout << "NEON support: " << (utils::has_neon_support() ? "Yes" : "No") << std::endl;
    
    // Define examples
    std::vector<std::pair<std::string, std::function<void()>>> examples = {
        {"Basic Generation", basic_generation_example},
        {"Advanced Configuration", advanced_configuration_example},
        {"Streaming Generation", streaming_generation_example},
        {"Batch Generation", batch_generation_example},
        {"Model Comparison", model_comparison_example},
        {"Performance Monitoring", performance_monitoring_example},
        {"Tensor Operations", tensor_operations_example},
        {"Quantization Config", quantization_example},
    };
    
    // Handle command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "interactive") {
            interactive_chat_example();
            return 0;
        } else if (arg == "help") {
            std::cout << "\nAvailable examples:" << std::endl;
            for (size_t i = 0; i < examples.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << examples[i].first << std::endl;
            }
            std::cout << "\nUsage:" << std::endl;
            std::cout << "  " << argv[0] << "           # Run all examples" << std::endl;
            std::cout << "  " << argv[0] << " interactive # Interactive chat" << std::endl;
            std::cout << "  " << argv[0] << " <number>    # Run specific example" << std::endl;
            return 0;
        } else {
            try {
                int example_num = std::stoi(arg) - 1;
                if (example_num >= 0 && example_num < static_cast<int>(examples.size())) {
                    std::cout << "\nRunning example: " << examples[example_num].first << std::endl;
                    examples[example_num].second();
                    return 0;
                } else {
                    std::cerr << "Invalid example number. Use 1-" << examples.size() << std::endl;
                    return 1;
                }
            } catch (const std::exception&) {
                std::cerr << "Invalid argument. Use 'help' for usage." << std::endl;
                return 1;
            }
        }
    }
    
    // Run all examples
    for (const auto& [name, func] : examples) {
        try {
            func();
        } catch (const std::exception& e) {
            std::cerr << "Error in " << name << ": " << e.what() << std::endl;
        }
        std::cout << std::string(50, '-') << std::endl;
    }
    
    return 0;
} 