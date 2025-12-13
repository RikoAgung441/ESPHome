const tempChanges = []

const pushToTempChanges = (key, value) => {
	const index = tempChanges.findIndex((item) => item.key === key)
	if (index !== -1) {
		tempChanges[index].value = value
	} else {
		tempChanges.push({ key, value })
	}
}

const getData = async () => {
	try {
		ws.on("connected", () => {
			ws.send("get-settings", {})
			ws.on("settings", (data) => {
				console.log("settings : ", data)
				if (!data) return

				setValue(data)
			})
		})

		ws.on("error", (error) => {
			console.log(error)
			showToast(error?.msg || "Failed to get data", true)
		})
	} catch (error) {
		showToast("Failed to fetch data from server", true)
	}
}

const setValue = (settings) => {
	const inputs = document.querySelectorAll("input[data-key]")
	inputs.forEach((input) => {
		const key = input.dataset.key
		// console.log(key, settings[key])
		if (settings[key]) {
			input.value = settings[key]
		}
	})
}

const btnScan = document.querySelector("#btn_scan_sta_ssid")
btnScan.addEventListener("click", async () => {
	// fungsi scan
	const contentWifiNearby = document.getElementById(
		"content_wifi_nearby_option"
	)

	contentWifiNearby.innerHTML = ""

	const dataDummy = [
		{ ssid: "Network 2", rssi: -34 },
		{ ssid: "Network 2", rssi: -34 },
		{ ssid: "Network 3", rssi: -34 },
		{ ssid: "Network 4", rssi: -34 },
		{ ssid: "Network 5", rssi: -34 },
	]

	dataDummy.forEach((data) => {
		const option = document.createElement("button")
		option.classList.add("wifi_nearby_option")
		option.dataset.ssid = data.ssid
		option.setAttribute("type", "button")
		option.addEventListener("click", () => {
			const ssid = option.dataset.ssid
			document.querySelector("#sta_ssid").value = ssid
			document.querySelector(".wifi_nearby").classList.remove("active")
			pushToTempChanges("sta_ssid", ssid)
		})

		option.innerHTML = `
			<p>${data.ssid}</p>
			<p>${data.rssi}dB</p>
		`
		contentWifiNearby.appendChild(option)
	})

	document.querySelector(".wifi_nearby").classList.add("active")
})

const btnClose = document.querySelector(".btn_close_wifi_nearby")
btnClose.addEventListener("click", () => {
	document.querySelector(".wifi_nearby").classList.remove("active")
})

const inputs = document.querySelectorAll("input[data-key]")
inputs.forEach((input) => {
	input.addEventListener("input", () => {
		const type = input.dataset.type

		if (type === "ip") {
			console.log(input.value)
			input.value = input.value.replace(/[^0-9.]/g, "")
		}

		const key = input.dataset.key
		const value = input.value
		pushToTempChanges(key, value)
		updateViewInput(input)
	})
})

const updateViewInput = (input) => {
	const key = input.dataset.key
	const index = tempChanges.findIndex((item) => item.key === key)
	if (index !== -1) {
		input.classList.add("border_input_changed")
	}
}

const btnSave = document.getElementById("btn_save_setting")
btnSave.addEventListener("click", async () => {
	console.log(tempChanges)
})

const hideAndShowPassword = (btnEye, elementInput) => {
	btnEye.addEventListener("click", () => {
		elementInput.type = elementInput.type === "password" ? "text" : "password"
		btnEye.querySelector("#iconEye").src =
			elementInput.type === "password"
				? "./asset/eye_off.svg"
				: "./asset/eye_show.svg"
	})
}

document.addEventListener("DOMContentLoaded", async () => {
	getData()

	hideAndShowPassword(
		document.getElementById("buttonEyeAP"),
		document.getElementById("ap_wifi_password")
	)

	hideAndShowPassword(
		document.getElementById("buttonEyeSTA"),
		document.getElementById("sta_wifi_password")
	)
})
