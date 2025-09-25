function goToRoom(element) {
	const roomNumber = element.dataset.room
	window.location.href = `room.html?room=${roomNumber}`
}

function goToSetting() {
	window.location.href = `setting.html`
}

const getRooms = async () => {
	try {
		const response = await fetch("/api/rooms")
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

// const getSettings = async () => {
// 	try {
// 		const response = await fetch("http://localhost:3000/settings")
// 		const data = await response.json()
// 		document.getElementById("ssid").innerText = data.ssid || ""
// 		document.getElementById("password").innerText = data.password || ""
// 	} catch (error) {
// 		showToast("Error fetching settings", true)
// 	}
// }

socket.onmessage = (event) => {
	const msg = JSON.parse(event.data)
	if (msg.event === "pzem-update") {
		// output.textContent = JSON.stringify(msg.data, null, 2)
		console.log(msg.data)
	}
}

document.getElementById("btnRelay").addEventListener("click", () => {
	ws.send(JSON.stringify({ event: "toggleRelay", data: {} }))
})

document.addEventListener("DOMContentLoaded", () => {
	getRooms()
	// getSettings()
})
