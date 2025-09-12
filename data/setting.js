const buttonEye = document.querySelector(".input_icon_eye")
const inputPassword = document.querySelector("#wifi_password")
const fields = document.querySelectorAll(".container_field_form")

fields.forEach((field) => {
	field.querySelector(".btn_set").addEventListener("click", async () => {
		const key = field.querySelector("input").dataset.key
		const value = field.querySelector("input").value

		try {
			const res = await fetch("/set", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({ key, value }),
			})

			const data = await res.json()

			console.log(data)
			if (data.ok) {
				showToast("Set successfully!")
			}
		} catch (error) {
			showToast("Failed to set data", true)
		}
	})
})

const getData = async () => {
	try {
		const res = await fetch("/api/settings", {
			method: "GET",
			headers: { "Content-Type": "application/json" },
		})
		const data = await res.json()
		console.log(data)
	} catch (error) {
		console.log("error test", error)
		showToast("Failed to fetch data from server", true)
	}
}

document.addEventListener("DOMContentLoaded", async () => {
	getData()

	buttonEye.addEventListener("click", () => {
		inputPassword.type = inputPassword.type === "password" ? "text" : "password"
		buttonEye.src =
			inputPassword.type === "password"
				? "./asset/eye_off.svg"
				: "./asset/eye_show.svg"
	})
})
