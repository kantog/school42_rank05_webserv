// Template configuratie
const siteConfig = {
    pages: [
        { name: 'Home', url: 'index.html'},
        { name: 'OefCorner', url: 'OefCorners.html' },
        { name: 'OefEdge', url: 'OefEdge.html' }
    ]
};

// Template functies
class RubikTemplate {
    constructor() {
        this.init();
    }

    init() {
        this.createNavigation();
        this.setupMobileMenu();
        this.setupPageDefaults();
    }

    setActivePage(pageName) {
        siteConfig.pages.forEach(page => {
            if (page.name === pageName) {
                page.active = true;
            } else {
                page.active = false;
            }
        });
        this.createNavigation();
    }

createNavigation() {
    const navMenu = document.getElementById('nav-menu');
    navMenu.innerHTML = '';

    const currentPath = window.location.pathname.split('/').pop(); // bijvoorbeeld 'index.html'

    siteConfig.pages.forEach(page => {
        const li = document.createElement('li');
        li.className = 'nav-item';

        const a = document.createElement('a');
        a.href = page.url;
        a.textContent = page.name;

        if (page.url === currentPath) {
            a.className = 'active';
        }

        li.appendChild(a);
        navMenu.appendChild(li);
    });
}


    setupMobileMenu() {
        const mobileMenu = document.getElementById('mobile-menu');
        const navMenu = document.getElementById('nav-menu');

        mobileMenu.addEventListener('click', () => {
            navMenu.classList.toggle('active');
        });
    }

    setupPageDefaults() {
        // Deze functie wordt overschreven door individuele pagina's
    }

    setPageContent(config) {
        // Update page title
        if (config.title) {
            document.getElementById('page-title').textContent = config.title;
            document.getElementById('main-title').textContent = config.title;
        }

        // Update subtitle
        if (config.subtitle) {
            document.getElementById('page-subtitle').textContent = config.subtitle;
        }

        // Update page content
        if (config.content) {
            document.getElementById('page-content').innerHTML = config.content;
        }

        // Show/hide canvas
        const canvasContainer = document.querySelector('.canvas-container');
        const controls = document.getElementById('cube-controls');

        if (config.showCube === false) {
            canvasContainer.style.display = 'none';
            controls.style.display = 'none';
        } else {
            canvasContainer.style.display = 'flex';
            controls.style.display = 'flex';
        }

        // Custom controls
        if (config.customControls) {
            controls.innerHTML = config.customControls;
        }

        // Mark active page in navigation
        if (config.activeMenu) {
            document.querySelectorAll('.nav-item a').forEach(a => {
                a.classList.remove('active');
                if (a.textContent === config.activeMenu) {
                    a.classList.add('active');
                }
            });
        }
    }
}

// Initialize template
const template = new RubikTemplate();

// Maak template beschikbaar voor andere scripts
window.RubikTemplate = template;