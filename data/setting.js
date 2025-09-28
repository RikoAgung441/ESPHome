const buttonEye = document.querySelector(".input_icon_eye")
const inputPassword = document.querySelector("#wifi_password")
const fields = document.querySelectorAll(".container_field_form")

fields.forEach((field) => {
	field.querySelector(".btn_set").addEventListener("click", async () => {
		const key = field.querySelector("input").dataset.key
		const value = field.querySelector("input").value

		try {
			const res = await fetch("/api/set", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({ key, value }),
			})

			const data = await res.json()

			if (data.ok) {
				showToast("Set successfully!")
			}
		} catch (error) {
			showToast("Failed to set data", true)
		}
	})
})

const setValue = (settings) => {
	const inputs = document.querySelectorAll("input[data-key]")
	inputs.forEach((input) => {
		const key = input.dataset.key
		if (settings[key]) {
			input.value = settings[key]
		}
	})
}

const getData = async () => {
	try {
		// const res = await fetch("/api/settings")
		// const data = await res.json()

		ws.on("connected", () => {
			ws.send("get-settings", {})
			ws.on("settings", (data) => {
				// console.log("settings : ", data.settings)
				setValue(data.settings)
			})
		})

		if (data) {
			setValue(data)
		}
		// console.log(data)
	} catch (error) {
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
