const buttonEye = document.querySelector(".input_icon_eye")
const inputSSID = document.querySelector("#SSID")
const inputPassword = document.querySelector("#wifi_password")
const fields = document.querySelectorAll(".container_field_form")

fields.forEach((field) => {
	field.querySelector(".btn_set").addEventListener("click", async () => {
		const key = field.querySelector("input").dataset.key
		const value = field.querySelector("input").value

		try {
			const res = await fetch("/save", {
				method: "POST",
				headers: { "Content-Type": "application/json" },
				body: JSON.stringify({ key, value }),
			})

			const data = await res.json()

			if (data.status === "success") {
				showToast("Set successfully!")
			}
		} catch (error) {
			showToast("Failed to set data", true)
		}
	})
})

function showToast(message, error = false, duration = 3000) {
	const container = document.getElementById("toast-container")

	const toast = document.createElement("div")
	toast.className = "toast"
	toast.innerText = message
	container.appendChild(toast)

	setTimeout(() => {
		error && toast.classList.add("error")
		toast.classList.add("show")
	}, 100)

	setTimeout(() => {
		toast.classList.remove("show")
		toast.classList.add("hide")

		toast.addEventListener("transitionend", () => toast.remove())
	}, duration)
}

const getData = async () => {
	try {
		const res = await fetch("/api/settings", {
			method: "GET",
			headers: { "Content-Type": "application/json" },
		})
		const data = await res.json()
		console.log(data)
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
