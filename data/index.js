// const { randomInt } = require("crypto")

function goToRoom(element) {
	const roomNumber = element.dataset.room
	window.location.href = `room.html?room=${roomNumber}`
}

function goToSetting() {
	window.location.href = `setting.html`
}

const getRooms = async () => {
	try {
		// const response = await fetch("/api/rooms")
		const response = await fetch("http://localhost:3000/rooms")

		const data = await response.json()
		console.log(data)
		const roomContainers = document.getElementById("rooms_container")
		data.forEach((room) => {
			const roomNumber = room.id
			const channelCount = room.channels.length || 0
			const name = room.name || `Room ${roomNumber}`

			roomContainers.innerHTML += `
				<button data-room="${roomNumber}" onclick="goToRoom(this)" class="room">
					<div class="header_room">
						<div>
							<div class="circle_icon">
								<img src="./asset/room.svg" alt="room">
							</div>
						</div>
						<div class="amount_channel">
							<h3>${name}</h3>
							<span> ${channelCount} channel</span>
						</div>
					</div>
        </button>
			`
		})
	} catch (error) {
		showToast("Error fetching rooms", true)
	}
}

const updatePzemData = (data) => {
	if (!data) return
	const keys = Object.keys(data)
	keys.forEach((key) => {
		const element = document.querySelector(`[data-key="${key}"]`)
		if (element) {
			element.textContent = `${data[key]} ${
				key === "voltage"
					? "V"
					: key === "current"
					? "A"
					: key === "power"
					? "W"
					: key === "energy"
					? "kWh"
					: ""
			}`
		}
	})
}

document.addEventListener("DOMContentLoaded", () => {
	getRooms()

	ws.on("pzem-update", (data) => {
		updatePzemData(data)
		console.log("PZEM Update:", data)
	})

	// ws.on("settings", (data) => {
	// 	console.log("Settings:", data)
	// })

	// const interval = setInterval(() => {
	// 	ws.send("get-settings", {
	// 		room: Math.random(2).toFixed(2),
	// 		channel: Math.random(2).toFixed(2),
	// 	})
	// }, 2000)

	// ws.on("disconnected", () => {
	// 	showToast("Disconnected", true)
	// 	clearInterval(interval)
	// })
	// getSettings()
})
