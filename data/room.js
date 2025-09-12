// import { showToast } from "./toast.js"

const totalChannels = 4
const channelContainers = document.getElementById("channel_list")

const getRoom = () => {
	const urlParams = new URLSearchParams(window.location.search)
	return urlParams.get("room") || ""
}

const setTitle = () => {
	const room = getRoom()
	const titleRoom = room ? `Room ${room}` : "Room"
	document.querySelector(".title").innerText = titleRoom
}

const createChannel = (id = 1) => {
	for (let i = 1; i <= totalChannels; i++) {
		channelContainers.innerHTML += `
		          <div class="channel">
            <div class="wrapper_content_channel">
              <div class="header_channel">
                <div class="circle_icon">
                  <img src="./asset/lamp.png" alt="lamp">
                </div>
                <span class="status">ON</span>
              </div>
              <div class="channel_content">
                <div class="channel_name">
                  <h3>Channel 2</h3>
                  <span>Room 1</span>
                </div>
                <label class="switch">
                  <input data-id="${id}" type="checkbox" onchange="switchChange(this, ${i})">
                  <span class="slider"></span>
                </label>
              </div>
            </div>
          </div>
		`
	}
}

const arrSwitchOn = []

const switchChange = (el, id) => {
	if (el.checked && !arrSwitchOn.includes(id)) {
		arrSwitchOn.push(id)
	} else if (!el.checked && arrSwitchOn.includes(id)) {
		const index = arrSwitchOn.indexOf(id)
		arrSwitchOn.splice(index, 1)
	}

	if (arrSwitchOn.length === totalChannels) {
		document.getElementById("all_on").checked = true
	} else {
		document.getElementById("all_on").checked = false
	}
}

const toggleAll = (el) => {
	const allSwitch = document.querySelectorAll(
		'.channel_content input[type="checkbox"]'
	)
	if (el.checked) {
		allSwitch.forEach((sw, i) => {
			console.log({ i })
			if (!arrSwitchOn.includes(i + 1) && i + 1 <= totalChannels) {
				arrSwitchOn.push(i + 1)
			}
			sw.checked = true
		})
	} else {
		allSwitch.forEach((sw, i) => {
			const index = arrSwitchOn.indexOf(i + 1)
			console.log({ index })
			if (index > -1) {
				arrSwitchOn.splice(index, 1)
			}
			sw.checked = false
		})
	}
}

const getChannel = async () => {
	try {
		const room = getRoom()
		const response = await fetch("/api/channels?room=" + room)
		const data = await response.json()
		console.log(data)
	} catch (error) {
		showToast(" Failed to get data", true)
		// console.error(error)
	}
}

const setSwitch = async (id, state) => {
	try {
		const room = getRoom()
		const response = await fetch("/api/channel", {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ id, room, state }),
		})
		const data = await response.json()
		console.log(data)
	} catch (error) {
		showToast(" Failed to set data", true)
		// console.error(error)
	}
}

document.addEventListener("DOMContentLoaded", () => {
	setTitle()
	getChannel()
	createChannel()
	// showToast("Welcome to Room Control Panel")
})
