// Internationalization System
// Author: Amelia Enora 🌈 Marceline Chavez Barroso
// Languages: Spanish, English, French, Thai, Japanese, Chinese

class I18nManager {
    constructor() {
        this.currentLanguage = localStorage.getItem('language') || 'es';
        this.translations = {};
        this.loadTranslations();
    }

    loadTranslations() {
        this.translations = {
            es: {
                // Spanish (original)
                title: "Amelia Enora 🌈 Marceline Chavez Barroso - CV Técnico | Systems & Assembly Developer",
                welcomeTitle: "BIENVENIDA AL CV TÉCNICO DE AMELIA ENORA 🌈 MARCELINE",
                role: "Desarrolladora de Sistemas y Assembly",
                expertise: "Especializada en programación de bajo nivel, optimización de algoritmos y sistemas embebidos. Experiencia en x86, x64, ARM con NASM y FASM.",
                
                // Navigation
                navAbout: "Sobre Mí",
                navSkills: "Habilidades", 
                navCertifications: "Certificaciones",
                navAlgorithms: "Algoritmos",
                navVisualizations: "Visualizaciones",
                navProjects: "Proyectos",
                navNetwork: "Herramientas de Red",
                
                // Sections
                programmingLanguages: "💻 Lenguajes de Programación",
                linuxDistributions: "🐧 Distribuciones Linux",
                cybersecurity: "🔐 Ciberseguridad",
                ciscoTitle: "🌐 Cisco CCNA",
                systemsTools: "🔧 Sistemas y Herramientas",
                algorithmsTitle: "🧮 Algoritmos y Matemáticas",
                
                // Certifications
                certificationsTitle: "🏆 Certificaciones y Licencias IT",
                informationSecurity: "🔐 Seguridad de la Información",
                networkingInfra: "🌐 Redes e Infraestructura", 
                cloudVirtualization: "☁️ Cloud y Virtualización",
                ethicalHacking: "🔍 Hacking Ético y Pentesting",
                projectManagement: "📊 Gestión de Proyectos y Negocios",
                blockchain: "🔗 Blockchain y Tecnologías Emergentes",
                
                // Algorithms
                algorithmsImplemented: "Algoritmos Implementados en WebAssembly",
                sortingShowcase: "🔥 Showcase de Algoritmos de Ordenamiento",
                selectAlgorithm: "Selecciona un algoritmo para ver la descripción",
                primeNumberSieve: "Criba de Números Primos",
                
                // Math Visualizations
                mathVisualizations: "Visualizaciones Matemáticas",
                mandelbrotFractals: "Fractales de Mandelbrot",
                fourierTransform: "Transformada de Fourier",
                juliaSet: "Conjunto de Julia",
                lorenzAttractor: "Atractor de Lorenz",
                primeSpirals: "Espirales de Números Primos",
                
                // Network Tools
                networkTools: "Herramientas de Red",
                packetAnalyzer: "Analizador de Paquetes",
                portScanner: "Escáner de Puertos",
                
                // Contact
                contact: "Desarrolladora de sistemas con pasión por la programación de bajo nivel.",
                
                // Theme
                changeTheme: "Cambiar tema",
                lightMode: "Cambiar a modo claro",
                darkMode: "Cambiar a modo oscuro"
            },
            
            en: {
                // English
                title: "Amelia Enora 🌈 Marceline Chavez Barroso - Technical CV | Systems & Assembly Developer",
                welcomeTitle: "WELCOME TO AMELIA ENORA 🌈 MARCELINE'S TECHNICAL CV",
                role: "Systems & Assembly Developer",
                expertise: "Specialized in low-level programming, algorithm optimization and embedded systems. Experience in x86, x64, ARM with NASM and FASM.",
                
                // Navigation
                navAbout: "About Me",
                navSkills: "Skills",
                navCertifications: "Certifications", 
                navAlgorithms: "Algorithms",
                navVisualizations: "Visualizations",
                navProjects: "Projects",
                navNetwork: "Network Tools",
                
                // Sections
                programmingLanguages: "💻 Programming Languages",
                linuxDistributions: "🐧 Linux Distributions",
                cybersecurity: "🔐 Cybersecurity",
                ciscoTitle: "🌐 Cisco CCNA",
                systemsTools: "🔧 Systems & Tools",
                algorithmsTitle: "🧮 Algorithms & Math",
                
                // Certifications
                certificationsTitle: "🏆 IT Certifications & Licenses",
                informationSecurity: "🔐 Information Security",
                networkingInfra: "🌐 Networking & Infrastructure",
                cloudVirtualization: "☁️ Cloud & Virtualization", 
                ethicalHacking: "🔍 Ethical Hacking & Penetration Testing",
                projectManagement: "📊 Project Management & Business",
                blockchain: "🔗 Blockchain & Emerging Technologies",
                
                // Algorithms
                algorithmsImplemented: "Algorithms Implemented in WebAssembly",
                sortingShowcase: "🔥 Sorting Algorithms Showcase",
                selectAlgorithm: "Select an algorithm to see description",
                primeNumberSieve: "Prime Number Sieve",
                
                // Math Visualizations
                mathVisualizations: "Mathematical Visualizations",
                mandelbrotFractals: "Mandelbrot Fractals",
                fourierTransform: "Fourier Transform",
                juliaSet: "Julia Set",
                lorenzAttractor: "Lorenz Attractor",
                primeSpirals: "Prime Number Spirals",
                
                // Network Tools
                networkTools: "Network Tools",
                packetAnalyzer: "Packet Analyzer", 
                portScanner: "Port Scanner",
                
                // Contact
                contact: "Systems developer with passion for low-level programming.",
                
                // Theme
                changeTheme: "Change theme",
                lightMode: "Switch to light mode",
                darkMode: "Switch to dark mode"
            },
            
            fr: {
                // French
                title: "Amelia Enora 🌈 Marceline Chavez Barroso - CV Technique | Développeuse Systèmes & Assembly",
                welcomeTitle: "BIENVENUE AU CV TECHNIQUE D'AMELIA ENORA 🌈 MARCELINE",
                role: "Développeuse Systèmes & Assembly",
                expertise: "Spécialisée en programmation bas niveau, optimisation d'algorithmes et systèmes embarqués. Expérience en x86, x64, ARM avec NASM et FASM.",
                
                // Navigation
                navAbout: "À Propos",
                navSkills: "Compétences",
                navCertifications: "Certifications",
                navAlgorithms: "Algorithmes", 
                navVisualizations: "Visualisations",
                navProjects: "Projets",
                navNetwork: "Outils Réseau",
                
                // Sections
                programmingLanguages: "💻 Langages de Programmation",
                linuxDistributions: "🐧 Distributions Linux",
                cybersecurity: "🔐 Cybersécurité",
                ciscoTitle: "🌐 Cisco CCNA",
                systemsTools: "🔧 Systèmes et Outils",
                algorithmsTitle: "🧮 Algorithmes et Mathématiques",
                
                // Certifications
                certificationsTitle: "🏆 Certifications et Licences IT",
                informationSecurity: "🔐 Sécurité de l'Information",
                networkingInfra: "🌐 Réseaux et Infrastructure",
                cloudVirtualization: "☁️ Cloud et Virtualisation",
                ethicalHacking: "🔍 Hacking Éthique et Tests de Pénétration",
                projectManagement: "📊 Gestion de Projet et Business",
                blockchain: "🔗 Blockchain et Technologies Émergentes",
                
                // Algorithms
                algorithmsImplemented: "Algorithmes Implémentés en WebAssembly",
                sortingShowcase: "🔥 Présentation des Algorithmes de Tri",
                selectAlgorithm: "Sélectionnez un algorithme pour voir la description",
                primeNumberSieve: "Crible des Nombres Premiers",
                
                // Math Visualizations
                mathVisualizations: "Visualisations Mathématiques",
                mandelbrotFractals: "Fractales de Mandelbrot",
                fourierTransform: "Transformée de Fourier",
                juliaSet: "Ensemble de Julia",
                lorenzAttractor: "Attracteur de Lorenz",
                primeSpirals: "Spirales de Nombres Premiers",
                
                // Network Tools
                networkTools: "Outils Réseau",
                packetAnalyzer: "Analyseur de Paquets",
                portScanner: "Scanner de Ports",
                
                // Contact
                contact: "Développeuse systèmes passionnée par la programmation bas niveau.",
                
                // Theme
                changeTheme: "Changer le thème",
                lightMode: "Passer au mode clair",
                darkMode: "Passer au mode sombre"
            },
            
            th: {
                // Thai
                title: "Amelia Enora 🌈 Marceline Chavez Barroso - เรซูเม่ทางเทคนิค | นักพัฒนาระบบและ Assembly",
                welcomeTitle: "ยินดีต้อนรับสู่เรซูเม่ทางเทคนิคของ AMELIA ENORA 🌈 MARCELINE",
                role: "นักพัฒนาระบบและ Assembly",
                expertise: "เชี่ยวชาญในการเขียนโปรแกรมระดับต่ำ การปรับปรุงอัลกอริทึม และระบบฝังตัว มีประสบการณ์ใน x86, x64, ARM ด้วย NASM และ FASM",
                
                // Navigation
                navAbout: "เกี่ยวกับฉัน",
                navSkills: "ทักษะ",
                navCertifications: "ใบรับรอง",
                navAlgorithms: "อัลกอริทึม",
                navVisualizations: "การแสดงภาพ",
                navProjects: "โครงการ",
                navNetwork: "เครื่องมือเครือข่าย",
                
                // Sections
                programmingLanguages: "💻 ภาษาการเขียนโปรแกรม",
                linuxDistributions: "🐧 การแจกจ่าย Linux",
                cybersecurity: "🔐 ความปลอดภัยทางไซเบอร์",
                ciscoTitle: "🌐 Cisco CCNA",
                systemsTools: "🔧 ระบบและเครื่องมือ",
                algorithmsTitle: "🧮 อัลกอริทึมและคณิตศาสตร์",
                
                // Certifications
                certificationsTitle: "🏆 ใบรับรองและใบอนุญาต IT",
                informationSecurity: "🔐 ความปลอดภัยของข้อมูล",
                networkingInfra: "🌐 เครือข่ายและโครงสร้างพื้นฐาน",
                cloudVirtualization: "☁️ คลาวด์และการจำลอง",
                ethicalHacking: "🔍 การแฮกเชิงจริยธรรมและการทดสอบการเจาะระบบ",
                projectManagement: "📊 การจัดการโครงการและธุรกิจ",
                blockchain: "🔗 บล็อกเชนและเทคโนโลยีใหม่ๆ",
                
                // Algorithms
                algorithmsImplemented: "อัลกอริทึมที่ใช้งานใน WebAssembly",
                sortingShowcase: "🔥 การนำเสนออัลกอริทึมการเรียงลำดับ",
                selectAlgorithm: "เลือกอัลกอริทึมเพื่อดูคำอธิบาย",
                primeNumberSieve: "ตะแกรงจำนวนเฉพาะ",
                
                // Math Visualizations
                mathVisualizations: "การแสดงภาพทางคณิตศาสตร์",
                mandelbrotFractals: "เศษส่วน Mandelbrot",
                fourierTransform: "การแปลงฟูเรียร์",
                juliaSet: "เซต Julia",
                lorenzAttractor: "ตัวดึงดูด Lorenz",
                primeSpirals: "เกลียวจำนวนเฉพาะ",
                
                // Network Tools
                networkTools: "เครื่องมือเครือข่าย",
                packetAnalyzer: "ตัววิเคราะห์แพ็กเก็ต",
                portScanner: "ตัวสแกนพอร์ต",
                
                // Contact
                contact: "นักพัฒนาระบบที่หลงใหลในการเขียนโปรแกรมระดับต่ำ",
                
                // Theme
                changeTheme: "เปลี่ยนธีม",
                lightMode: "เปลี่ยนเป็นโหมดสว่าง",
                darkMode: "เปลี่ยนเป็นโหมดมืด"
            },
            
            ja: {
                // Japanese
                title: "Amelia Enora 🌈 Marceline Chavez Barroso - 技術履歴書 | システム・アセンブリ開発者",
                welcomeTitle: "AMELIA ENORA 🌈 MARCELINE の技術履歴書へようこそ",
                role: "システム・アセンブリ開発者",
                expertise: "低レベルプログラミング、アルゴリズム最適化、組み込みシステムを専門としています。x86、x64、ARM で NASM と FASM の経験があります。",
                
                // Navigation
                navAbout: "私について",
                navSkills: "スキル",
                navCertifications: "認定資格",
                navAlgorithms: "アルゴリズム",
                navVisualizations: "可視化",
                navProjects: "プロジェクト",
                navNetwork: "ネットワークツール",
                
                // Sections
                programmingLanguages: "💻 プログラミング言語",
                linuxDistributions: "🐧 Linux ディストリビューション",
                cybersecurity: "🔐 サイバーセキュリティ",
                ciscoTitle: "🌐 Cisco CCNA",
                systemsTools: "🔧 システムとツール",
                algorithmsTitle: "🧮 アルゴリズムと数学",
                
                // Certifications
                certificationsTitle: "🏆 IT認定とライセンス",
                informationSecurity: "🔐 情報セキュリティ",
                networkingInfra: "🌐 ネットワークとインフラストラクチャ",
                cloudVirtualization: "☁️ クラウドと仮想化",
                ethicalHacking: "🔍 エシカルハッキングとペネトレーションテスト",
                projectManagement: "📊 プロジェクト管理とビジネス",
                blockchain: "🔗 ブロックチェーンと新興技術",
                
                // Algorithms
                algorithmsImplemented: "WebAssembly で実装されたアルゴリズム",
                sortingShowcase: "🔥 ソートアルゴリズムショーケース",
                selectAlgorithm: "説明を見るためにアルゴリズムを選択してください",
                primeNumberSieve: "素数の篩",
                
                // Math Visualizations
                mathVisualizations: "数学的可視化",
                mandelbrotFractals: "マンデルブロ集合",
                fourierTransform: "フーリエ変換",
                juliaSet: "ジュリア集合",
                lorenzAttractor: "ローレンツアトラクタ",
                primeSpirals: "素数螺旋",
                
                // Network Tools
                networkTools: "ネットワークツール",
                packetAnalyzer: "パケットアナライザー",
                portScanner: "ポートスキャナー",
                
                // Contact
                contact: "低レベルプログラミングに情熱を持つシステム開発者。",
                
                // Theme
                changeTheme: "テーマを変更",
                lightMode: "ライトモードに切り替え",
                darkMode: "ダークモードに切り替え"
            },
            
            zh: {
                // Chinese (Simplified)
                title: "Amelia Enora 🌈 Marceline Chavez Barroso - 技术简历 | 系统与汇编开发者",
                welcomeTitle: "欢迎来到 AMELIA ENORA 🌈 MARCELINE 的技术简历",
                role: "系统与汇编开发者",
                expertise: "专门从事底层编程、算法优化和嵌入式系统。在 x86、x64、ARM 方面拥有使用 NASM 和 FASM 的经验。",
                
                // Navigation
                navAbout: "关于我",
                navSkills: "技能",
                navCertifications: "认证",
                navAlgorithms: "算法",
                navVisualizations: "可视化",
                navProjects: "项目",
                navNetwork: "网络工具",
                
                // Sections
                programmingLanguages: "💻 编程语言",
                linuxDistributions: "🐧 Linux 发行版",
                cybersecurity: "🔐 网络安全",
                ciscoTitle: "🌐 思科 CCNA",
                systemsTools: "🔧 系统和工具",
                algorithmsTitle: "🧮 算法和数学",
                
                // Certifications
                certificationsTitle: "🏆 IT 认证和许可证",
                informationSecurity: "🔐 信息安全",
                networkingInfra: "🌐 网络和基础设施",
                cloudVirtualization: "☁️ 云计算和虚拟化",
                ethicalHacking: "🔍 道德黑客和渗透测试",
                projectManagement: "📊 项目管理和商业",
                blockchain: "🔗 区块链和新兴技术",
                
                // Algorithms
                algorithmsImplemented: "在 WebAssembly 中实现的算法",
                sortingShowcase: "🔥 排序算法展示",
                selectAlgorithm: "选择算法查看描述",
                primeNumberSieve: "质数筛",
                
                // Math Visualizations
                mathVisualizations: "数学可视化",
                mandelbrotFractals: "曼德布洛集",
                fourierTransform: "傅里叶变换",
                juliaSet: "朱利亚集",
                lorenzAttractor: "洛伦兹吸引子",
                primeSpirals: "质数螺旋",
                
                // Network Tools
                networkTools: "网络工具",
                packetAnalyzer: "数据包分析器",
                portScanner: "端口扫描器",
                
                // Contact
                contact: "热衷于底层编程的系统开发者。",
                
                // Theme
                changeTheme: "更换主题",
                lightMode: "切换到浅色模式",
                darkMode: "切换到深色模式"
            }
        };
    }

    init() {
        document.addEventListener('DOMContentLoaded', () => {
            this.setupLanguageSelector();
            this.translatePage();
        });
    }

    setupLanguageSelector() {
        const select = document.getElementById('language-select');
        if (select) {
            select.value = this.currentLanguage;
            select.addEventListener('change', (e) => {
                this.changeLanguage(e.target.value);
            });
        }
    }

    changeLanguage(lang) {
        this.currentLanguage = lang;
        localStorage.setItem('language', lang);
        this.translatePage();
        
        // Update page title
        document.title = this.translations[lang].title;
        
        // Dispatch language change event
        window.dispatchEvent(new CustomEvent('languageChanged', { 
            detail: { language: lang } 
        }));
    }

    translatePage() {
        const lang = this.currentLanguage;
        const t = this.translations[lang];
        
        if (!t) return;

        // Update all elements with data-i18n attribute
        document.querySelectorAll('[data-i18n]').forEach(element => {
            const key = element.getAttribute('data-i18n');
            if (t[key]) {
                element.textContent = t[key];
            }
        });

        // Update specific elements by ID/class
        this.updateElement('title', t.title);
        this.updateElement('.terminal-title', `amelia@systems:~/cv$`);
        
        // Update navigation links
        this.updateElement('a[href="#about"]', `./${t.navAbout?.toLowerCase() || 'about'}.sh`);
        this.updateElement('a[href="#skills"]', `./${t.navSkills?.toLowerCase() || 'skills'}.asm`);
        
        // Update section titles
        const sectionUpdates = {
            '#about h1': t.role,
            '.skills-grid h3:nth-of-type(1)': t.programmingLanguages,
            '.skills-grid h3:nth-of-type(2)': t.linuxDistributions,
            '.certifications-grid h3': t.certificationsTitle
        };
        
        Object.entries(sectionUpdates).forEach(([selector, text]) => {
            this.updateElement(selector, text);
        });
    }

    updateElement(selector, text) {
        const element = document.querySelector(selector);
        if (element && text) {
            element.textContent = text;
        }
    }

    t(key) {
        return this.translations[this.currentLanguage]?.[key] || key;
    }

    getCurrentLanguage() {
        return this.currentLanguage;
    }
}

// Initialize i18n manager
const i18nManager = new I18nManager();
i18nManager.init();

// Make it globally available
window.i18nManager = i18nManager; 