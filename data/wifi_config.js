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
	field.querySelector(".btn_set").addEventListener("click", async () => {
		const id = field.querySelector("input").id
		const value = field.querySelector("input").value

		const res = await fetch("/api/setting", {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ key: id, value }),
		})

		const data = await res.json()

		if (data.status === "success") {
			alert("Success")
		}
		console.log(data)
	})
})
