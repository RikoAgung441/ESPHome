const buttonEye = document.querySelector(".input_icon_eye")
const inputSSID = document.querySelector("#SSID")
const inputPassword = document.querySelector("#input_wifi_password")
// const btnSubmit = document.querySelector(".btn_submit")

buttonEye.addEventListener("click", () => {
	inputPassword.type = inputPassword.type === "password" ? "text" : "password"
	buttonEye.src =
		inputPassword.type === "password"
			? "./asset/eye_off.svg"
			: "./asset/eye_show.svg"
})

const fields = document.querySelectorAll(".container_field_form")
fields.forEach((field) => {
	field.querySelector(".btn_set").addEventListener("click", async () => {
		const key = field.querySelector("input").dataset.key
		const value = field.querySelector("input").value

		const res = await fetch("/save", {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ key, value }),
		})

		const data = await res.json()

		if (data.status === "success") {
			alert("Success")
		}
		console.log(data)
	})
})

document.addEventListener("DOMContentLoaded", async () => {
	const res = await fetch("/api/test", {
		method: "POST",
		headers: { "Content-Type": "application/json" },
		body: JSON.stringify({}),
	})
	const data = await res.json()
	console.log(data)
	// inputSSID.value = data.ssid
	// inputPassword.value = data.password
	// document.querySelector("#wifi_name").textContent = data.ssid
	// document.querySelector("#wifi_password").textContent = data.password
	// document.querySelector("#wifi_status").textContent = data.status
	// document.querySelector("#wifi_ip").textContent = data.ip
})
