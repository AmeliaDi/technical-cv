#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include "../src/arm_llm_runtime.h"

namespace py = pybind11;
using namespace arm_llm;

PYBIND11_MODULE(arm_llm_runtime, m) {
    m.doc() = "ARM LLM Runtime - Fast LLM inference engine for ARM architectures";
    
    // Enums
    py::enum_<QuantizationType>(m, "QuantizationType")
        .value("NONE", QuantizationType::NONE)
        .value("Q4_0", QuantizationType::Q4_0)
        .value("Q4_K", QuantizationType::Q4_K)
        .value("Q8_0", QuantizationType::Q8_0)
        .value("Q8_K", QuantizationType::Q8_K)
        .value("CUSTOM", QuantizationType::CUSTOM);
    
    py::enum_<ModelType>(m, "ModelType")
        .value("LLAMA", ModelType::LLAMA)
        .value("GPT", ModelType::GPT)
        .value("MISTRAL", ModelType::MISTRAL)
        .value("GEMMA", ModelType::GEMMA)
        .value("PHI", ModelType::PHI)
        .value("MULTIMODAL", ModelType::MULTIMODAL);
    
    py::enum_<DataType>(m, "DataType")
        .value("FLOAT32", DataType::FLOAT32)
        .value("FLOAT16", DataType::FLOAT16)
        .value("INT8", DataType::INT8)
        .value("INT4", DataType::INT4);
    
    // Configuration structures
    py::class_<QuantizationConfig>(m, "QuantizationConfig")
        .def(py::init<>())
        .def_readwrite("method", &QuantizationConfig::method)
        .def_readwrite("bits", &QuantizationConfig::bits)
        .def_readwrite("group_size", &QuantizationConfig::group_size)
        .def_readwrite("calibration_dataset", &QuantizationConfig::calibration_dataset)
        .def_readwrite("symmetric", &QuantizationConfig::symmetric)
        .def_readwrite("scale_factor", &QuantizationConfig::scale_factor);
    
    py::class_<RuntimeConfig>(m, "RuntimeConfig")
        .def(py::init<>())
        .def_readwrite("memory_pool_size", &RuntimeConfig::memory_pool_size)
        .def_readwrite("enable_mmap", &RuntimeConfig::enable_mmap)
        .def_readwrite("enable_kv_cache_compression", &RuntimeConfig::enable_kv_cache_compression)
        .def_readwrite("num_threads", &RuntimeConfig::num_threads)
        .def_readwrite("use_thread_pool", &RuntimeConfig::use_thread_pool)
        .def_readwrite("use_neon", &RuntimeConfig::use_neon)
        .def_readwrite("use_flash_attention", &RuntimeConfig::use_flash_attention)
        .def_readwrite("enable_speculative_decoding", &RuntimeConfig::enable_speculative_decoding)
        .def_readwrite("cache_dir", &RuntimeConfig::cache_dir)
        .def_readwrite("max_cache_size", &RuntimeConfig::max_cache_size)
        .def_readwrite("max_batch_size", &RuntimeConfig::max_batch_size)
        .def_readwrite("max_context_length", &RuntimeConfig::max_context_length)
        .def_readwrite("temperature", &RuntimeConfig::temperature)
        .def_readwrite("top_k", &RuntimeConfig::top_k)
        .def_readwrite("top_p", &RuntimeConfig::top_p);
    
    py::class_<GenerationConfig>(m, "GenerationConfig")
        .def(py::init<>())
        .def_readwrite("max_tokens", &GenerationConfig::max_tokens)
        .def_readwrite("temperature", &GenerationConfig::temperature)
        .def_readwrite("top_k", &GenerationConfig::top_k)
        .def_readwrite("top_p", &GenerationConfig::top_p)
        .def_readwrite("repetition_penalty", &GenerationConfig::repetition_penalty)
        .def_readwrite("stop_sequences", &GenerationConfig::stop_sequences)
        .def_readwrite("stream", &GenerationConfig::stream)
        .def_readwrite("seed", &GenerationConfig::seed);
    
    py::class_<ModelInfo>(m, "ModelInfo")
        .def(py::init<>())
        .def_readwrite("name", &ModelInfo::name)
        .def_readwrite("path", &ModelInfo::path)
        .def_readwrite("type", &ModelInfo::type)
        .def_readwrite("size_bytes", &ModelInfo::size_bytes)
        .def_readwrite("quantization", &ModelInfo::quantization)
        .def_readwrite("vocab_size", &ModelInfo::vocab_size)
        .def_readwrite("hidden_size", &ModelInfo::hidden_size)
        .def_readwrite("num_layers", &ModelInfo::num_layers)
        .def_readwrite("num_heads", &ModelInfo::num_heads)
        .def_readwrite("context_length", &ModelInfo::context_length)
        .def_readwrite("metadata", &ModelInfo::metadata);
    
    py::class_<GenerationResult>(m, "GenerationResult")
        .def(py::init<>())
        .def_readwrite("text", &GenerationResult::text)
        .def_readwrite("logits", &GenerationResult::logits)
        .def_readwrite("tokens_generated", &GenerationResult::tokens_generated)
        .def_readwrite("generation_time", &GenerationResult::generation_time)
        .def_readwrite("tokens_per_second", &GenerationResult::tokens_per_second)
        .def_readwrite("memory_used", &GenerationResult::memory_used);
    
    // Performance metrics
    py::class_<ArmLLMRuntime::PerformanceMetrics>(m, "PerformanceMetrics")
        .def_readwrite("total_inference_time", &ArmLLMRuntime::PerformanceMetrics::total_inference_time)
        .def_readwrite("avg_tokens_per_second", &ArmLLMRuntime::PerformanceMetrics::avg_tokens_per_second)
        .def_readwrite("total_tokens_generated", &ArmLLMRuntime::PerformanceMetrics::total_tokens_generated)
        .def_readwrite("memory_used", &ArmLLMRuntime::PerformanceMetrics::memory_used)
        .def_readwrite("cache_hits", &ArmLLMRuntime::PerformanceMetrics::cache_hits)
        .def_readwrite("cache_misses", &ArmLLMRuntime::PerformanceMetrics::cache_misses);
    
    // Tensor class
    py::class_<Tensor>(m, "Tensor")
        .def(py::init<const std::vector<int64_t>&, DataType>())
        .def("shape", &Tensor::shape, py::return_value_policy::reference_internal)
        .def("dtype", &Tensor::dtype)
        .def("size", &Tensor::size)
        .def("bytes", &Tensor::bytes)
        .def("zero", &Tensor::zero)
        .def("fill", &Tensor::fill)
        .def("reshape", &Tensor::reshape)
        .def("data", [](Tensor& self) {
            // Convert to numpy array based on dtype
            auto shape = self.shape();
            switch (self.dtype()) {
                case DataType::FLOAT32:
                    return py::array_t<float>(
                        shape,
                        static_cast<float*>(self.data())
                    );
                case DataType::FLOAT16:
                    return py::array_t<uint16_t>(
                        shape,
                        static_cast<uint16_t*>(self.data())
                    );
                case DataType::INT8:
                    return py::array_t<int8_t>(
                        shape,
                        static_cast<int8_t*>(self.data())
                    );
                default:
                    throw std::runtime_error("Unsupported data type for numpy conversion");
            }
        });
    
    // Model class
    py::class_<Model, std::shared_ptr<Model>>(m, "Model")
        .def("generate", &Model::generate,
             "Generate text from a prompt",
             py::arg("prompt"), py::arg("config") = GenerationConfig{})
        .def("generate_batch", &Model::generateBatch,
             "Generate text from multiple prompts",
             py::arg("prompts"), py::arg("config") = GenerationConfig{})
        .def("generate_stream", [](Model& self, const std::string& prompt, py::function callback, const GenerationConfig& config) {
            self.generateStream(prompt, [callback](const std::string& token) {
                callback(token);
            }, config);
        }, "Generate text with streaming output",
           py::arg("prompt"), py::arg("callback"), py::arg("config") = GenerationConfig{})
        .def("info", &Model::info, py::return_value_policy::reference_internal)
        .def("tokenize", &Model::tokenize)
        .def("detokenize", &Model::detokenize);
    
    // Main runtime class
    py::class_<ArmLLMRuntime>(m, "ArmLLMRuntime")
        .def(py::init<const RuntimeConfig&>(), py::arg("config") = RuntimeConfig{})
        .def("load_model", &ArmLLMRuntime::loadModel,
             "Load a model from HuggingFace Hub",
             py::arg("model_id"), py::arg("quantization") = QuantizationType::Q4_K)
        .def("load_model_from_file", &ArmLLMRuntime::loadModelFromFile,
             "Load a model from local file",
             py::arg("model_path"), py::arg("quantization") = QuantizationType::Q4_K)
        .def("unload_model", &ArmLLMRuntime::unloadModel)
        .def("unload_all_models", &ArmLLMRuntime::unloadAllModels)
        .def("quantize_model", &ArmLLMRuntime::quantizeModel,
             "Quantize a model",
             py::arg("input_path"), py::arg("output_path"), py::arg("config"))
        .def("configure", &ArmLLMRuntime::configure)
        .def("config", &ArmLLMRuntime::config, py::return_value_policy::reference_internal)
        .def("list_cached_models", &ArmLLMRuntime::listCachedModels)
        .def("clear_cache", &ArmLLMRuntime::clearCache)
        .def("get_metrics", &ArmLLMRuntime::getMetrics)
        .def("reset_metrics", &ArmLLMRuntime::resetMetrics);
    
    // Utility functions
    py::module utils = m.def_submodule("utils", "Utility functions");
    utils.def("get_system_info", &utils::get_system_info);
    utils.def("has_neon_support", &utils::has_neon_support);
    utils.def("get_available_memory", &utils::get_available_memory);
    utils.def("format_bytes", &utils::format_bytes);
    utils.def("get_time_ms", &utils::get_time_ms);
    
    // High-level convenience functions
    m.def("load_model", [](const std::string& model_id, const std::string& quantize = "Q4_K") {
        static ArmLLMRuntime runtime;
        QuantizationType q_type = QuantizationType::Q4_K;
        
        if (quantize == "Q4_0") q_type = QuantizationType::Q4_0;
        else if (quantize == "Q4_K") q_type = QuantizationType::Q4_K;
        else if (quantize == "Q8_0") q_type = QuantizationType::Q8_0;
        else if (quantize == "Q8_K") q_type = QuantizationType::Q8_K;
        else if (quantize == "NONE") q_type = QuantizationType::NONE;
        
        return runtime.loadModel(model_id, q_type);
    }, "Load a model with default runtime configuration",
       py::arg("model_id"), py::arg("quantize") = "Q4_K");
    
    m.def("generate", [](const std::string& model_id, const std::string& prompt, 
                        int max_tokens = 100, float temperature = 0.7f, const std::string& quantize = "Q4_K") {
        static ArmLLMRuntime runtime;
        QuantizationType q_type = QuantizationType::Q4_K;
        
        if (quantize == "Q4_0") q_type = QuantizationType::Q4_0;
        else if (quantize == "Q4_K") q_type = QuantizationType::Q4_K;
        else if (quantize == "Q8_0") q_type = QuantizationType::Q8_0;
        else if (quantize == "Q8_K") q_type = QuantizationType::Q8_K;
        else if (quantize == "NONE") q_type = QuantizationType::NONE;
        
        auto model = runtime.loadModel(model_id, q_type);
        
        GenerationConfig config;
        config.max_tokens = max_tokens;
        config.temperature = temperature;
        
        return model->generate(prompt, config);
    }, "Generate text with a single function call",
       py::arg("model_id"), py::arg("prompt"), py::arg("max_tokens") = 100, 
       py::arg("temperature") = 0.7f, py::arg("quantize") = "Q4_K");
    
    // Version info
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "ARM LLM Runtime Team";
    
    // Examples in docstring
    m.doc() = R"pbdoc(
        ARM LLM Runtime Python Bindings
        
        Fast and efficient Large Language Model inference engine optimized for ARM architectures.
        
        Examples:
            Basic usage:
            
            >>> import arm_llm_runtime as llm
            >>> model = llm.load_model("microsoft/DialoGPT-medium")
            >>> response = model.generate("Hello, how are you?")
            >>> print(response.text)
            
            Advanced usage:
            
            >>> runtime = llm.ArmLLMRuntime()
            >>> model = runtime.load_model("meta-llama/Llama-2-7b-chat-hf", llm.QuantizationType.Q4_K)
            >>> 
            >>> config = llm.GenerationConfig()
            >>> config.max_tokens = 200
            >>> config.temperature = 0.8
            >>> config.top_p = 0.9
            >>> 
            >>> result = model.generate("Explain quantum computing", config)
            >>> print(f"Generated {result.tokens_generated} tokens in {result.generation_time:.2f}s")
            >>> print(f"Speed: {result.tokens_per_second:.1f} tokens/s")
            
            Streaming generation:
            
            >>> def on_token(token):
            ...     print(token, end='', flush=True)
            >>> 
            >>> model.generate_stream("Tell me a story", on_token)
            
            Batch generation:
            
            >>> prompts = ["What is AI?", "How does ML work?", "Explain neural networks"]
            >>> results = model.generate_batch(prompts)
            >>> for result in results:
            ...     print(result.text)
    )pbdoc";
} 