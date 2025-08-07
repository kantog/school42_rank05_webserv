const slider = document.getElementById("speedSlider");
const valueDisplay = document.getElementById("speedValue");

// Laad opgeslagen waarde (als die er is)
let savedSpeed = localStorage.getItem("simSpeed");
if (savedSpeed !== null) {
  slider.value = savedSpeed;
  valueDisplay.textContent = savedSpeed;
}

// Update opslag als gebruiker slider beweegt
slider.addEventListener("input", function () {
  const speed = this.value;
  localStorage.setItem("simSpeed", speed);
  valueDisplay.textContent = speed;
});