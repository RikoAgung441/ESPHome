const buttonEye = document.querySelector(".input_icon_eye")
const inputSSID = document.querySelector("#SSID")
const inputPassword = document.querySelector("#input_wifi_password")
// const btnSubmit = document.querySelector(".btn_submit")

buttonEye.addEventListener("click", () => {
	inputPassword.type = inputPassword.type === "password" ? "text" : "password"
	buttonEye.src =
		inputPassword.type === "password"
			? "../asset/eye-off.svg"
			: "../asset/eye-show.svg"
})

const fields = document.querySelectorAll(".container_field_form")
fields.forEach((field) => {
	field.querySelector(".btn_set").addEventListener("click", () => {
		console.log(field.querySelector("label").textContent)
		console.log(field.querySelector("input").value)
	})
})

console.log(fields)
