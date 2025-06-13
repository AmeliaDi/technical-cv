#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <signal.h>
#include <getopt.h>
#include "../src/arm_llm_runtime.h"

using namespace arm_llm;

// Global variables for signal handling
std::unique_ptr<ArmLLMRuntime> g_runtime;
std::shared_ptr<Model> g_current_model;
volatile bool g_running = true;

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nReceived interrupt signal. Shutting down gracefully...\n";
        g_running = false;
        if (g_runtime) {
            g_runtime->unloadAllModels();
        }
        exit(0);
    }
}

void print_usage(const char* program_name) {
    std::cout << "ARM LLM Runtime - Fast LLM inference on ARM devices\n\n";
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -m, --model MODEL_ID          HuggingFace model ID (required)\n";
    std::cout << "  -p, --prompt PROMPT            Input prompt for generation\n";
    std::cout << "  -i, --interactive              Interactive chat mode\n";
    std::cout << "  -q, --quantize TYPE            Quantization type (Q4_0, Q4_K, Q8_0, Q8_K)\n";
    std::cout << "  -t, --max-tokens N             Maximum tokens to generate (default: 100)\n";
    std::cout << "  -T, --temperature F            Temperature for sampling (default: 0.7)\n";
    std::cout << "  -k, --top-k N                  Top-k sampling (default: 50)\n";
    std::cout << "  -P, --top-p F                  Top-p sampling (default: 0.9)\n";
    std::cout << "  -s, --stream                   Stream output tokens\n";
    std::cout << "  -b, --batch-size N             Batch size for inference (default: 1)\n";
    std::cout << "  -n, --threads N                Number of threads (default: auto)\n";
    std::cout << "  -c, --cache-dir DIR            Model cache directory (default: ./models)\n";
    std::cout << "  -M, --memory-size SIZE         Memory pool size (default: 8GB)\n";
    std::cout << "  --profile                      Enable performance profiling\n";
    std::cout << "  --verbose                      Verbose output\n";
    std::cout << "  --list-models                  List cached models\n";
    std::cout << "  --clear-cache                  Clear model cache\n";
    std::cout << "  --system-info                  Show system information\n";
    std::cout << "  -h, --help                     Show this help message\n";
    std::cout << "  -v, --version                  Show version information\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  # Run a simple generation\n";
    std::cout << "  " << program_name << " -m microsoft/DialoGPT-medium -p \"Hello, how are you?\"\n\n";
    
    std::cout << "  # Interactive chat mode\n";
    std::cout << "  " << program_name << " -m microsoft/DialoGPT-medium -i\n\n";
    
    std::cout << "  # Use quantization for memory efficiency\n";
    std::cout << "  " << program_name << " -m meta-llama/Llama-2-7b-chat-hf -q Q4_K -p \"Explain AI\"\n\n";
    
    std::cout << "  # Stream output with custom parameters\n";
    std::cout << "  " << program_name << " -m microsoft/DialoGPT-medium -p \"Tell me a story\" -s -t 200 -T 0.8\n\n";
}

void print_version() {
    std::cout << "ARM LLM Runtime v1.0.0\n";
    std::cout << "Built for ARM architectures with NEON optimizations\n";
    std::cout << "HuggingFace integration enabled\n";
    std::cout << utils::get_system_info() << std::endl;
}

void print_system_info() {
    std::cout << utils::get_system_info() << std::endl;
}

QuantizationType parse_quantization(const std::string& type) {
    if (type == "Q4_0") return QuantizationType::Q4_0;
    if (type == "Q4_K") return QuantizationType::Q4_K;
    if (type == "Q8_0") return QuantizationType::Q8_0;
    if (type == "Q8_K") return QuantizationType::Q8_K;
    if (type == "NONE") return QuantizationType::NONE;
    
    std::cerr << "Warning: Unknown quantization type '" << type << "', using Q4_K\n";
    return QuantizationType::Q4_K;
}

size_t parse_memory_size(const std::string& size_str) {
    size_t size = std::stoull(size_str);
    
    // Check for suffix
    if (size_str.back() == 'B' || size_str.back() == 'b') {
        std::string suffix = size_str.substr(size_str.length() - 2);
        if (suffix == "GB" || suffix == "gb") {
            size *= 1024 * 1024 * 1024;
        } else if (suffix == "MB" || suffix == "mb") {
            size *= 1024 * 1024;
        } else if (suffix == "KB" || suffix == "kb") {
            size *= 1024;
        }
    } else if (isalpha(size_str.back())) {
        char suffix = std::toupper(size_str.back());
        if (suffix == 'G') {
            size *= 1024 * 1024 * 1024;
        } else if (suffix == 'M') {
            size *= 1024 * 1024;
        } else if (suffix == 'K') {
            size *= 1024;
        }
    }
    
    return size;
}

void run_interactive_mode(std::shared_ptr<Model> model, const GenerationConfig& gen_config) {
    std::cout << "\n=== ARM LLM Runtime Interactive Mode ===\n";
    std::cout << "Type 'quit', 'exit', or press Ctrl+C to exit\n";
    std::cout << "Type 'help' for commands\n\n";
    
    std::string input;
    while (g_running) {
        std::cout << "You: ";
        std::getline(std::cin, input);
        
        if (input.empty()) continue;
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input == "help") {
            std::cout << "\nCommands:\n";
            std::cout << "  help    - Show this help\n";
            std::cout << "  quit    - Exit interactive mode\n";
            std::cout << "  exit    - Exit interactive mode\n";
            std::cout << "  clear   - Clear conversation history\n";
            std::cout << "  info    - Show model information\n";
            std::cout << "  stats   - Show performance statistics\n\n";
            continue;
        }
        
        if (input == "clear") {
            std::cout << "Conversation history cleared.\n\n";
            continue;
        }
        
        if (input == "info") {
            const auto& info = model->info();
            std::cout << "\nModel Information:\n";
            std::cout << "  Name: " << info.name << "\n";
            std::cout << "  Type: " << static_cast<int>(info.type) << "\n";
            std::cout << "  Quantization: " << static_cast<int>(info.quantization) << "\n";
            std::cout << "  Vocab size: " << info.vocab_size << "\n";
            std::cout << "  Hidden size: " << info.hidden_size << "\n";
            std::cout << "  Layers: " << info.num_layers << "\n";
            std::cout << "  Heads: " << info.num_heads << "\n";
            std::cout << "  Context length: " << info.context_length << "\n\n";
            continue;
        }
        
        if (input == "stats") {
            auto metrics = g_runtime->getMetrics();
            std::cout << "\nPerformance Statistics:\n";
            std::cout << "  Total inference time: " << metrics.total_inference_time << "s\n";
            std::cout << "  Average tokens/sec: " << metrics.avg_tokens_per_second << "\n";
            std::cout << "  Total tokens generated: " << metrics.total_tokens_generated << "\n";
            std::cout << "  Memory used: " << utils::format_bytes(metrics.memory_used) << "\n";
            std::cout << "  Cache hits: " << metrics.cache_hits << "\n";
            std::cout << "  Cache misses: " << metrics.cache_misses << "\n\n";
            continue;
        }
        
        // Generate response
        std::cout << "Assistant: ";
        std::cout.flush();
        
        if (gen_config.stream) {
            model->generateStream(input, [](const std::string& token) {
                std::cout << token << std::flush;
            }, gen_config);
            std::cout << "\n\n";
        } else {
            auto result = model->generate(input, gen_config);
            // Extract just the generated part (after the input prompt)
            std::string response = result.text.substr(input.length());
            std::cout << response << "\n\n";
            
            // Show generation stats
            std::cout << "  [Generated " << result.tokens_generated << " tokens in " 
                      << std::fixed << std::setprecision(2) << result.generation_time 
                      << "s, " << result.tokens_per_second << " tokens/s]\n\n";
        }
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Command line options
    std::string model_id;
    std::string prompt;
    bool interactive = false;
    QuantizationType quantization = QuantizationType::Q4_K;
    bool stream = false;
    bool profile = false;
    bool verbose = false;
    bool list_models = false;
    bool clear_cache = false;
    bool show_system_info = false;
    
    GenerationConfig gen_config;
    RuntimeConfig runtime_config;
    
    // Parse command line arguments
    static struct option long_options[] = {
        {"model", required_argument, 0, 'm'},
        {"prompt", required_argument, 0, 'p'},
        {"interactive", no_argument, 0, 'i'},
        {"quantize", required_argument, 0, 'q'},
        {"max-tokens", required_argument, 0, 't'},
        {"temperature", required_argument, 0, 'T'},
        {"top-k", required_argument, 0, 'k'},
        {"top-p", required_argument, 0, 'P'},
        {"stream", no_argument, 0, 's'},
        {"batch-size", required_argument, 0, 'b'},
        {"threads", required_argument, 0, 'n'},
        {"cache-dir", required_argument, 0, 'c'},
        {"memory-size", required_argument, 0, 'M'},
        {"profile", no_argument, 0, 0},
        {"verbose", no_argument, 0, 0},
        {"list-models", no_argument, 0, 0},
        {"clear-cache", no_argument, 0, 0},
        {"system-info", no_argument, 0, 0},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "m:p:iq:t:T:k:P:sb:n:c:M:hv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'm':
                model_id = optarg;
                break;
            case 'p':
                prompt = optarg;
                break;
            case 'i':
                interactive = true;
                break;
            case 'q':
                quantization = parse_quantization(optarg);
                break;
            case 't':
                gen_config.max_tokens = std::stoi(optarg);
                break;
            case 'T':
                gen_config.temperature = std::stof(optarg);
                break;
            case 'k':
                gen_config.top_k = std::stoi(optarg);
                break;
            case 'P':
                gen_config.top_p = std::stof(optarg);
                break;
            case 's':
                stream = true;
                gen_config.stream = true;
                break;
            case 'b':
                runtime_config.max_batch_size = std::stoi(optarg);
                break;
            case 'n':
                runtime_config.num_threads = std::stoi(optarg);
                break;
            case 'c':
                runtime_config.cache_dir = optarg;
                break;
            case 'M':
                runtime_config.memory_pool_size = parse_memory_size(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'v':
                print_version();
                return 0;
            case 0:
                // Long options
                if (strcmp(long_options[option_index].name, "profile") == 0) {
                    profile = true;
                } else if (strcmp(long_options[option_index].name, "verbose") == 0) {
                    verbose = true;
                } else if (strcmp(long_options[option_index].name, "list-models") == 0) {
                    list_models = true;
                } else if (strcmp(long_options[option_index].name, "clear-cache") == 0) {
                    clear_cache = true;
                } else if (strcmp(long_options[option_index].name, "system-info") == 0) {
                    show_system_info = true;
                }
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Handle special commands
    if (show_system_info) {
        print_system_info();
        return 0;
    }
    
    try {
        // Initialize runtime
        g_runtime = std::make_unique<ArmLLMRuntime>(runtime_config);
        
        if (verbose) {
            std::cout << "Initialized ARM LLM Runtime with:\n";
            std::cout << "  Memory pool: " << utils::format_bytes(runtime_config.memory_pool_size) << "\n";
            std::cout << "  Threads: " << runtime_config.num_threads << "\n";
            std::cout << "  Cache dir: " << runtime_config.cache_dir << "\n";
            std::cout << "  NEON enabled: " << (runtime_config.use_neon ? "Yes" : "No") << "\n\n";
        }
        
        if (list_models) {
            auto models = g_runtime->listCachedModels();
            std::cout << "Cached models:\n";
            for (const auto& model : models) {
                std::cout << "  " << model << "\n";
            }
            return 0;
        }
        
        if (clear_cache) {
            g_runtime->clearCache();
            std::cout << "Model cache cleared.\n";
            return 0;
        }
        
        // Validate required arguments
        if (model_id.empty()) {
            std::cerr << "Error: Model ID is required. Use -m or --model to specify.\n";
            print_usage(argv[0]);
            return 1;
        }
        
        if (!interactive && prompt.empty()) {
            std::cerr << "Error: Either prompt (-p) or interactive mode (-i) is required.\n";
            print_usage(argv[0]);
            return 1;
        }
        
        // Load model
        if (verbose) {
            std::cout << "Loading model: " << model_id << "\n";
            std::cout << "Quantization: " << static_cast<int>(quantization) << "\n";
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        g_current_model = g_runtime->loadModel(model_id, quantization);
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto load_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        if (verbose) {
            std::cout << "Model loaded in " << load_duration.count() << "ms\n";
            const auto& info = g_current_model->info();
            std::cout << "Model info: " << info.vocab_size << " vocab, " 
                      << info.num_layers << " layers, " 
                      << info.context_length << " context\n\n";
        }
        
        // Run inference
        if (interactive) {
            run_interactive_mode(g_current_model, gen_config);
        } else {
            // Single prompt mode
            if (stream) {
                std::cout << "Response: ";
                g_current_model->generateStream(prompt, [](const std::string& token) {
                    std::cout << token << std::flush;
                }, gen_config);
                std::cout << "\n";
            } else {
                auto result = g_current_model->generate(prompt, gen_config);
                std::cout << "Response: " << result.text << "\n";
                
                if (verbose || profile) {
                    std::cout << "\nGeneration Statistics:\n";
                    std::cout << "  Tokens generated: " << result.tokens_generated << "\n";
                    std::cout << "  Generation time: " << std::fixed << std::setprecision(3) 
                              << result.generation_time << "s\n";
                    std::cout << "  Tokens per second: " << std::fixed << std::setprecision(1) 
                              << result.tokens_per_second << "\n";
                    std::cout << "  Memory used: " << utils::format_bytes(result.memory_used) << "\n";
                }
            }
        }
        
        if (profile) {
            auto metrics = g_runtime->getMetrics();
            std::cout << "\nRuntime Performance Metrics:\n";
            std::cout << "  Total inference time: " << metrics.total_inference_time << "s\n";
            std::cout << "  Average tokens/sec: " << metrics.avg_tokens_per_second << "\n";
            std::cout << "  Total tokens generated: " << metrics.total_tokens_generated << "\n";
            std::cout << "  Memory used: " << utils::format_bytes(metrics.memory_used) << "\n";
            std::cout << "  Cache hits: " << metrics.cache_hits << "\n";
            std::cout << "  Cache misses: " << metrics.cache_misses << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 