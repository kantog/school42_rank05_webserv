const canvas = document.getElementById('cubeCanvas');
const ctx = canvas.getContext('2d');

const angle = 70 * Math.PI / 180; // Convert to radians

const colors = {
    'v': [0, 255, 0], 'U': [0, 255, 0], 'u': [0, 255, 0], 'V': [0, 255, 0], '3': [0, 255, 0], 'X': [0, 255, 0], 'w': [0, 255, 0], 'W': [0, 255, 0], 'x': [0, 255, 0],
    'f': [255, 255, 0], 'F': [255, 255, 0], 'g': [255, 255, 0], 'E': [255, 255, 0], '5': [255, 255, 0], 'G': [255, 255, 0], 'e': [255, 255, 0], 'H': [255, 255, 0], 'h': [255, 255, 0],
    'm': [255, 0, 0], 'M': [255, 0, 0], 'n': [255, 0, 0], 'P': [255, 0, 0], '4': [255, 0, 0], 'N': [255, 0, 0], 'p': [255, 0, 0], 'O': [255, 0, 0], 'o': [255, 0, 0],
    's': [0, 0, 255], 'S': [0, 0, 255], 't': [0, 0, 255], 'R': [0, 0, 255], '1': [0, 0, 255], 'T': [0, 0, 255], 'r': [0, 0, 255], 'Q': [0, 0, 255], 'q': [0, 0, 255],
    'b': [255, 140, 0], 'A': [255, 140, 0], 'a': [255, 140, 0], 'B': [255, 140, 0], '2': [255, 140, 0], 'D': [255, 140, 0], 'c': [255, 140, 0], 'C': [255, 140, 0], 'd': [255, 140, 0],
    'i': [255, 255, 255], 'L': [255, 255, 255], 'l': [255, 255, 255], 'I': [255, 255, 255], '6': [255, 255, 255], 'K': [255, 255, 255], 'j': [255, 255, 255], 'J': [255, 255, 255], 'k': [255, 255, 255]
};

class CubeRect {
    constructor(pos, plane, coord, scale, key = 'l') {
        this.length = 100 * scale;
        this.scale = scale;
        this.key = key;
        this.orgKey = key;
        this.newKey = null;

        // Calculate 3D position projected to 2D
        this.pos = [
            coord[0] + pos[0] * this.length * Math.sin(angle) - pos[2] * this.length * Math.sin(angle),
            coord[1] + pos[1] * this.length - pos[0] * this.length * Math.cos(angle) - pos[2] * this.length * Math.cos(angle)
        ];

        this.plusPos = null;
        this.points = null;
        this.color = null;

        this[plane]();
    }

    front() {
        const l = this.length;
        this.color = [255, 255, 255];
        this.points = [
            [l * 1.2, l * 2],
            [l * 1.2, l],
            [l * 1.2 + Math.sin(angle) * l, l - Math.cos(angle) * l],
            [l * 1.2 + Math.sin(angle) * l, 2 * l - Math.cos(angle) * l]
        ];
    }

    left() {
        const l = this.length;
        this.color = [255, 140, 0];
        this.points = [
            [l * 1.2 - Math.sin(angle) * l, 2 * l - Math.cos(angle) * l],
            [l * 1.2 - Math.sin(angle) * l, l - Math.cos(angle) * l],
            [l * 1.2, l],
            [l * 1.2, l * 2]
        ];
    }

    top() {
        const l = this.length;
        this.color = [0, 0, 255];
        this.points = [
            [l * 1.2, l],
            [l * 1.2 - Math.sin(angle) * l, l - Math.cos(angle) * l],
            [l * 1.2, l - 2 * Math.cos(angle) * l],
            [l * 1.2 + Math.sin(angle) * l, l - Math.cos(angle) * l]
        ];
    }

    back() {
        const l = this.length;
        this.color = [255, 255, 0];
        this.points = [
            [l * 1.2 - Math.sin(angle) * l, 2 * l - Math.cos(angle) * l],
            [l * 1.2 - Math.sin(angle) * l, l - Math.cos(angle) * l],
            [l * 1.2, l - 2 * Math.cos(angle) * l],
            [l * 1.2, l * 2 - 2 * Math.cos(angle) * l]
        ];
        this.plusPos = [this.pos[0] - 300 * this.scale, this.pos[1] - 30 * this.scale];
    }

    bottom() {
        const l = this.length;
        this.color = [0, 255, 0];
        this.points = [
            [l * 1.2, l * 2],
            [l * 1.2 - Math.sin(angle) * l, 2 * l - Math.cos(angle) * l],
            [l * 1.2, l * 2 - 2 * Math.cos(angle) * l],
            [l * 1.2 + Math.sin(angle) * l, 2 * l - Math.cos(angle) * l]
        ];
        this.plusPos = [this.pos[0], this.pos[1] + 200 * this.scale];
    }

    right() {
        const l = this.length;
        this.color = [255, 0, 0];
        this.points = [
            [l * 1.2, l * 2 - 2 * Math.cos(angle) * l],
            [l * 1.2, l - 2 * Math.cos(angle) * l],
            [l * 1.2 + Math.sin(angle) * l, l - Math.cos(angle) * l],
            [l * 1.2 + Math.sin(angle) * l, 2 * l - Math.cos(angle) * l]
        ];
        this.plusPos = [this.pos[0] + 300 * this.scale, this.pos[1] - 30 * this.scale];
    }

    newBlock(key) {
        this.newKey = key;
    }

    update() {
        if (this.newKey !== null) {
            this.key = this.newKey;
            this.newKey = null;
        }
    }

    draw(ctx) {
        const currentColor = colors[this.key] || this.color;

        ctx.save();
        ctx.translate(this.pos[0], this.pos[1]);

        // Draw the polygon
        ctx.beginPath();
        ctx.moveTo(this.points[0][0], this.points[0][1]);
        for (let i = 1; i < this.points.length; i++) {
            ctx.lineTo(this.points[i][0], this.points[i][1]);
        }
        ctx.closePath();

        ctx.fillStyle = `rgba(${currentColor[0]}, ${currentColor[1]}, ${currentColor[2]}, 0.9)`;
        ctx.fill();
        ctx.strokeStyle = 'black';
        ctx.lineWidth = 1;
        ctx.stroke();

        ctx.restore();

        // Draw additional position if exists
        if (this.plusPos) {
            ctx.save();
            ctx.translate(this.plusPos[0], this.plusPos[1]);

            ctx.beginPath();
            ctx.moveTo(this.points[0][0], this.points[0][1]);
            for (let i = 1; i < this.points.length; i++) {
                ctx.lineTo(this.points[i][0], this.points[i][1]);
            }
            ctx.closePath();

            ctx.fillStyle = `rgba(${currentColor[0]}, ${currentColor[1]}, ${currentColor[2]}, 0.9)`;
            ctx.fill();
            ctx.strokeStyle = 'black';
            ctx.lineWidth = 1;
            ctx.stroke();

            ctx.restore();
        }
    }
}

class RubiksCube {
    constructor(scale = 1.0, position = [500, 150]) {
        this.position = position;
        this.scale = scale;
        this.rects = {};
        this.keys = [];
        this.moves = [];
        this.tick = 0;

        this.initializeCube();
    }

    setNieuwRect(pos, plane, key) {
        this.keys.push(key);
        this.rects[key] = new CubeRect(pos, plane, this.position, this.scale, key);
    }

    initializeCube() {
        // Bottom face
        this.setNieuwRect([0, 2, 0], 'bottom', 'v');
        this.setNieuwRect([0, 2, 1], 'bottom', 'U');
        this.setNieuwRect([0, 2, 2], 'bottom', 'u');
        this.setNieuwRect([1, 2, 0], 'bottom', 'V');
        this.setNieuwRect([1, 2, 1], 'bottom', '3');
        this.setNieuwRect([1, 2, 2], 'bottom', 'X');
        this.setNieuwRect([2, 2, 0], 'bottom', 'w');
        this.setNieuwRect([2, 2, 1], 'bottom', 'W');
        this.setNieuwRect([2, 2, 2], 'bottom', 'x');

        // Back face
        this.setNieuwRect([0, 0, 2], 'back', 'f');
        this.setNieuwRect([0, 1, 2], 'back', 'F');
        this.setNieuwRect([0, 2, 2], 'back', 'g');
        this.setNieuwRect([1, 0, 2], 'back', 'E');
        this.setNieuwRect([1, 1, 2], 'back', '5');
        this.setNieuwRect([1, 2, 2], 'back', 'G');
        this.setNieuwRect([2, 0, 2], 'back', 'e');
        this.setNieuwRect([2, 1, 2], 'back', 'H');
        this.setNieuwRect([2, 2, 2], 'back', 'h');

        // Right face
        this.setNieuwRect([2, 0, 0], 'right', 'm');
        this.setNieuwRect([2, 0, 1], 'right', 'M');
        this.setNieuwRect([2, 0, 2], 'right', 'n');
        this.setNieuwRect([2, 1, 0], 'right', 'P');
        this.setNieuwRect([2, 1, 1], 'right', '4');
        this.setNieuwRect([2, 1, 2], 'right', 'N');
        this.setNieuwRect([2, 2, 0], 'right', 'p');
        this.setNieuwRect([2, 2, 1], 'right', 'O');
        this.setNieuwRect([2, 2, 2], 'right', 'o');

        // Top face
        this.setNieuwRect([0, 0, 0], 'top', 's');
        this.setNieuwRect([0, 0, 1], 'top', 'S');
        this.setNieuwRect([0, 0, 2], 'top', 't');
        this.setNieuwRect([1, 0, 0], 'top', 'R');
        this.setNieuwRect([1, 0, 1], 'top', '1');
        this.setNieuwRect([1, 0, 2], 'top', 'T');
        this.setNieuwRect([2, 0, 0], 'top', 'r');
        this.setNieuwRect([2, 0, 1], 'top', 'Q');
        this.setNieuwRect([2, 0, 2], 'top', 'q');

        // Left face
        this.setNieuwRect([0, 0, 0], 'left', 'b');
        this.setNieuwRect([0, 0, 1], 'left', 'A');
        this.setNieuwRect([0, 0, 2], 'left', 'a');
        this.setNieuwRect([0, 1, 0], 'left', 'B');
        this.setNieuwRect([0, 1, 1], 'left', '2');
        this.setNieuwRect([0, 1, 2], 'left', 'D');
        this.setNieuwRect([0, 2, 0], 'left', 'c');
        this.setNieuwRect([0, 2, 1], 'left', 'C');
        this.setNieuwRect([0, 2, 2], 'left', 'd');

        // Front face
        this.setNieuwRect([0, 0, 0], 'front', 'i');
        this.setNieuwRect([0, 1, 0], 'front', 'L');
        this.setNieuwRect([0, 2, 0], 'front', 'l');
        this.setNieuwRect([1, 0, 0], 'front', 'I');
        this.setNieuwRect([1, 1, 0], 'front', '6');
        this.setNieuwRect([1, 2, 0], 'front', 'K');
        this.setNieuwRect([2, 0, 0], 'front', 'j');
        this.setNieuwRect([2, 1, 0], 'front', 'J');
        this.setNieuwRect([2, 2, 0], 'front', 'k');
    }

    swap(i, planes) {
        // Store new keys for each block
        for (const plane of planes) {
            for (let blockIndex = 0; blockIndex < plane.length; blockIndex++) {

                const blockName = plane[blockIndex];
                const targetIndex = (blockIndex + i + 4) % 4;
                const targetBlock = plane[targetIndex];
                this.rects[blockName].newBlock(this.rects[targetBlock].key);
            }
        }

        // Update all blocks
        for (const plane of planes) {
            for (const blockName of plane) {
                this.rects[blockName].update();
            }
        }
    }

    u(i) {
        this.swap(i, ["aime", "AIME", "bjnf", "tsrq", "TSRQ"]);
    }

    r(i) {
        this.swap(-i, ["brpv", "BRPV", "csmw", "ijkl", "IJKL"]);
    }

    d(i) {
        this.swap(-i, ["dlph", "CKOG", "ckog", "UVWX", "uvwx"]);
    }

    l(i) {
        this.swap(-i, ["qauo", "TDXN", "tdxn", "efgh", "EFGH"]);
    }

    f(i) {
        this.swap(-i, ["abcd", "ABCD", "tivg", "SLUF", "sluf"]);
    }

    b(i) {
        this.swap(i, ["mnop", "MNOP", "rkxe", "QJWH", "qjwh"]);
    }

    dd(i) {
        this.d(i)
        this.swap(-i, ["2645", "DLPH", "BJNF"])
    }

    ll(i) {
        this.l(i)
        this.swap(-i, ["1234", "QAUO", "SCWM"])
    }

    doMoves(movesString, instant = false, reverse = false) {
        if (!movesString) return;

        let moves;
        if (reverse) {
            moves = movesString.split(' ').filter(move => move.length > 0).reverse();
            let reverseAr = [];
            for (const move of moves) {
                let comand = move[0].toLowerCase();

                let i = 1
                if (move.length > 1 && /[a-z]/i.test(move[1])) {
                    comand += move[1].toLowerCase()
                    i += 1
                }
                let direction;
                if (move.length == i) {
                    direction = "'"
                }
                else {
                    if (move[i] == "'") {
                        direction = "";
                    } else {
                        direction = "2";
                    }
                }
                reverseAr.push(comand + direction)
            }
            moves = reverseAr

        }
        else {
            moves = movesString.split(' ').filter(move => move.length > 0);
        }
        this.moves.push(...moves);

        if (instant) {
            while (this.moves.length > 0) {
                this.executeMove();
            }
        }
    }

    doBlind(setups, key, alg) {
        this.doMoves(setups[key]);
        this.doMoves(alg);
        this.doMoves(setups[key], false, true);
    }

executeMove() {
    if (this.moves.length === 0) return;

    const move = this.moves.shift();

    let command = move[0].toLowerCase();
    let i = 1;

    if (move.length > 1 && /[a-zA-Z]/.test(move[1])) {
        command += move[1].toLowerCase();
        i += 1;
    }

    let direction;
    if (move.length === i) {
        direction = 1;
    } else if (move[i] === "'") {
        direction = -1;
    } else if (move[i] === "2") {
        direction = 2;
    } else {
        direction = 69;
    }

    if (typeof this[command] === 'function') {
        this[command](direction);
    } else {
        console.warn(`Unknown move command: ${command}`);
    }
}

    scramble() {
        const moves = ['R', 'U', 'F', 'D', 'L', 'B'];
        const modifiers = ['', "'", '2'];
        let scramble = '';

        for (let i = 0; i < 20; i++) {
            const move = moves[Math.floor(Math.random() * moves.length)];
            const modifier = modifiers[Math.floor(Math.random() * modifiers.length)];
            scramble += move + modifier + ' ';
        }
        this.doMoves(scramble, false);
    }

    reset() {
        this.moves = [];
        for (const key in this.rects) {
            this.rects[key].key = this.rects[key].orgKey;
        }
    }

    update() {
        if (this.moves.length > 0) {
            this.tick++;
            if (this.tick >= localStorage.getItem("simSpeed") || 0) { // Slower animation
                this.tick = 0;
                this.executeMove();
            }
        }
    }

    draw(ctx) {
        this.update();

        // Clear canvas
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        // Draw all rectangles
        for (const key of this.keys) {
            this.rects[key].draw(ctx);
        }
    }
}

// Initialize 400
const cube = new RubiksCube(0.65, [325, 100]);

// Animation loop
function animate() {
    cube.draw(ctx);
    requestAnimationFrame(animate);
}

// Keyboard controls
document.addEventListener('keydown', (event) => {
    const key = event.key.toLowerCase();
    const isShift = event.shiftKey;

    let move = '';
    switch (key) {
        case 'r': move = 'R'; break;
        case 'u': move = 'U'; break;
        case 'f': move = 'F'; break;
        case 'd': move = 'D'; break;
        case 'l': move = 'L'; break;
        case 'b': move = 'B'; break;
    }

    if (move) {
        if (isShift) move += "'";
        cube.doMoves(move);
        event.preventDefault();
    }
});

// Start animation
animate();
