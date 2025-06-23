const corner_setup_moves = {
    " ": "",
    "a": "F R'",
    "b": "R'",
    "c": "R' D'",
    "d": "D2, R",
    "e": "",
    "f": "F' D",
    "g": "F'",
    "h": "D' R",
    "i": "F",
    "j": "R' F",
    "k": "R2 F",
    "l": "f D",
    "m": "R D'",
    "n": "",
    "o": "D2 R' D'",
    "p": "R",
    "q": "",
    "r": "R2",
    "s": "R2 D'",
    "t": "F2",
    "u": "D",
    "v": "",
    "w": "D'",
    "x": "D2",
};

const edge_setup_moves = {
    " ": "",
    "a": "LL D' L2",
    "b": "DD2 L",
    "c": "D' L DD L'",
    "d": "L'",
    "e": "L' DD L'",
    "f": "DD' l",
    "g": "l DD L'",
    "h": "DD L'",
    "i": "",
    "j": "DD L",
    "k": "DD2 l DD L'",
    "l": "DD' L'",
    "m": "LL' D L2",
    "n": "L",
    "o": "D l DD L'",
    "p": "DD2 L'",
    "q": "LL' D l DD L'",
    "r": "",
    "s": "LL D' l DD L'",
    "t": "",
    "u": "D' L2",
    "v": "D2 L2",
    "w": "D L2",
    "x": "L2",
};

class CubeTrainer {
    constructor() {
        this.isRunning = false;
        this.startTime = 0;
        this.currentTime = 0;
        this.mode = 'speed';
        this.times = this.loadTimes();

        this.setupEventListeners();
        this.updateDisplay();
    }

    setupEventListeners() {
        // Keyboard events
        document.addEventListener('keydown', (e) => {
            if (e.code === 'Space') {
                e.preventDefault();
                this.toggleTimer();
            } else if (e.code === 'Enter') {
                e.preventDefault();
                this.saveTime();
            }
        });

        // Button events
        document.getElementById('startBtn').addEventListener('click', () => this.toggleTimer());
        document.getElementById('saveBtn').addEventListener('click', () => this.saveTime());
        document.getElementById('resetBtn').addEventListener('click', () => this.resetData());
        document.getElementById('mode').addEventListener('change', (e) => this.changeMode(e.target.value));
    }

    toggleTimer() {
        if (!this.isRunning) {
            this.startTimer();
        } else {
            this.stopTimer();
        }
    }

    startTimer() {
        this.isRunning = true;
        this.startTime = Date.now();
        this.updateButtons();
        this.timerInterval = setInterval(() => this.updateTimer(), 10);
    }

    stopTimer() {
        this.isRunning = false;
        clearInterval(this.timerInterval);
        this.updateButtons();
    }

    updateTimer() {
        this.currentTime = (Date.now() - this.startTime) / 1000;
        document.getElementById('timer').textContent = this.currentTime.toFixed(3);
    }

    saveTime() {
        if (this.currentTime > 0) {
            if (!this.times[this.mode]) {
                this.times[this.mode] = [];
            }

            this.times[this.mode].push({
                time: this.currentTime,
                date: new Date().toISOString()
            });

            this.saveTimes();
            this.updateDisplay();
            this.resetTimer();
        }
    }

    resetTimer() {
        this.currentTime = 0;
        document.getElementById('timer').textContent = '0.000';
        this.updateButtons();
    }

    changeMode(newMode) {
        this.mode = newMode;
        this.resetTimer();
        this.updateDisplay();
    }

    updateButtons() {
        const startBtn = document.getElementById('startBtn');
        const saveBtn = document.getElementById('saveBtn');

        if (this.isRunning) {
            startBtn.textContent = 'Stop Timer (SPATIE)';
            saveBtn.disabled = true;
        } else {
            startBtn.textContent = 'Start Timer (SPATIE)';
            saveBtn.disabled = this.currentTime === 0;
        }
    }

    updateDisplay() {
        const modeTimes = this.times[this.mode] || [];

        // Update stats
        if (modeTimes.length > 0) {
            const times = modeTimes.map(t => t.time).sort((a, b) => a - b);

            document.getElementById('bestTime').textContent = times[0].toFixed(3) + 's';
            document.getElementById('totalSolves').textContent = times.length;

            // Average of 5
            if (times.length >= 5) {
                const recent5 = modeTimes.slice(-5).map(t => t.time).sort((a, b) => a - b);
                const avg5 = (recent5.slice(1, 4).reduce((a, b) => a + b, 0) / 3);
                document.getElementById('avg5').textContent = avg5.toFixed(3) + 's';
            } else {
                document.getElementById('avg5').textContent = '-';
            }

            // Average of 12
            if (times.length >= 12) {
                const recent12 = modeTimes.slice(-12).map(t => t.time).sort((a, b) => a - b);
                const avg12 = (recent12.slice(1, 11).reduce((a, b) => a + b, 0) / 10);
                document.getElementById('avg12').textContent = avg12.toFixed(3) + 's';
            } else {
                document.getElementById('avg12').textContent = '-';
            }
        } else {
            document.getElementById('bestTime').textContent = '-';
            document.getElementById('avg5').textContent = '-';
            document.getElementById('avg12').textContent = '-';
            document.getElementById('totalSolves').textContent = '0';
        }

        // Update history
        this.updateHistory();
    }

    updateHistory() {
        const history = document.getElementById('history');
        const modeTimes = this.times[this.mode] || [];

        if (modeTimes.length === 0) {
            history.innerHTML = '<p>Nog geen tijden opgeslagen...</p>';
            return;
        }

        const times = modeTimes.map(t => t.time);
        const best = Math.min(...times);
        const avg = times.reduce((a, b) => a + b, 0) / times.length;

        history.innerHTML = '';

        // Show last 10 times
        modeTimes.slice(-10).reverse().forEach(entry => {
            const div = document.createElement('div');
            div.className = 'time-entry';

            if (entry.time === best) {
                div.classList.add('best');
            } else if (entry.time < avg) {
                div.classList.add('good');
            } else {
                div.classList.add('bad');
            }

            div.innerHTML = `
                        <span>${entry.time.toFixed(3)}s</span>
                        <span>${new Date(entry.date).toLocaleTimeString()}</span>
                    `;

            history.appendChild(div);
        });
    }

    // Simple localStorage voor nu - later database
    loadTimes() {
        const saved = localStorage.getItem('cubeTrainerTimes');
        return saved ? JSON.parse(saved) : {};
    }

    saveTimes() {
        localStorage.setItem('cubeTrainerTimes', JSON.stringify(this.times));
    }

    resetData() {
        if (confirm('Weet je zeker dat je alle data wilt wissen?')) {
            this.times = {};
            this.saveTimes();
            this.updateDisplay();
            this.resetTimer();
        }
    }
}

// Start the application
const trainer = new CubeTrainer();

let _setSetupMoves;

let currentMove = null;
let isPressed = false;

const subtitle = document.getElementById("page-subtitle");
const title = document.getElementById("main-title");
const saveBtn = document.getElementById("saveBtn");
const resetBtn = document.getElementById("resetBtn");
const timeList = document.getElementById("timeList");


function resetAll() {
    timerRunning = false;
    elapsed = 0;
    subtitle.textContent = "time: 0.000";
    title.textContent = "Corener: ?";
    saveBtn.disabled = true;
    currentMove = null;
    cube.reset();
}

// Grijs maken bij indrukken
function handlePressStart() {
    event.preventDefault();
    if (!isPressed && !timerRunning) {
        canvas.style.backgroundColor = "#ccc";
    }
    isPressed = true;
}

function getRandomMove() {
    const startCharCode = 'a'.charCodeAt(0); // 97
    const endCharCode = 'x'.charCodeAt(0);   // 120
    const randomCharCode = Math.floor(Math.random() * (endCharCode - startCharCode + 1)) + startCharCode;
    return String.fromCharCode(randomCharCode);
}

// Loslaten start/stopt de timer
function handlePressEnd() {
    event.preventDefault();
    if (!isPressed) return;
    canvas.style.backgroundColor = ""; // reset grijs

    if (!timerRunning) {
        currentMove = getRandomMove();
        title.textContent = `${title.textContent.split(":")[0]}: ${currentMove}`;
        startTimer();
    } else {
        stopTimer();
        saveBtn.disabled = false;
        cube.doBlind(_setSetupMoves, currentMove, alg);
    }

    timerRunning = !timerRunning;
    isPressed = false;
}

// Event listeners voor muis en touch
canvas.addEventListener("mousedown", handlePressStart);
canvas.addEventListener("mouseup", handlePressEnd);
canvas.addEventListener("touchstart", handlePressStart, { passive: false });
canvas.addEventListener("touchend", handlePressEnd, { passive: false });

let alg = "";



function setSetupMoves(name) {
    if (name == "corners") {
        _setSetupMoves = corner_setup_moves;
        alg = "R U' R' U' R U R' F' R U R' U' R' F R";
    }
    else {
        _setSetupMoves = edge_setup_moves;
        alg = "R U R' U' R' F R2 U' R' U' R U R' F'";
    }
}
