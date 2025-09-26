let totalChannels = 0
const channelContainers = document.getElementById("channel_list")

const getIdUrlParams = () => {
	const urlParams = new URLSearchParams(window.location.search)
	return urlParams.get("room") || ""
}

const setTitle = () => {
	const room = getIdUrlParams()
	const titleRoom = room ? `Room ${room}` : "Room"
	document.querySelector(".title").innerText = titleRoom
}

const createChannel = (channels) => {
	channelContainers.innerHTML = ""
	channels.forEach((channel) => {
		const id = channel.id
		const name = channel.name || `Channel ${id}`
		const room = channel.room || "Unknown"
		const status = channel.status ? "ON" : "OFF"
		const checked = channel.status ? "checked" : ""

		channelContainers.innerHTML += `
			<div class="channel">
				<div class="wrapper_content_channel">
					<div class="header_channel">
						<div class="circle_icon">
							<img src="./asset/lamp.png" alt="lamp">
						</div>
						<span class="status">${status}</span>
					</div>
					<div class="channel_content">
						<div class="channel_name">
							<h3>${name}</h3>
							<span>Room ${room}</span>
						</div>
						<label class="switch">
							<input data-id="${id}" type="checkbox" onchange="switchChange(this, ${id})" 
							${checked}>
							<span class="slider"></span>
						</label>
					</div>
				</div>
	    </div>
		`
	})
}

const arrSwitchOn = []

const switchChange = (el, id) => {
	if (el.checked && !arrSwitchOn.includes(id)) {
		arrSwitchOn.push(id)
	} else if (!el.checked && arrSwitchOn.includes(id)) {
		const index = arrSwitchOn.indexOf(id)
		arrSwitchOn.splice(index, 1)
	}

	changeAll(arrSwitchOn.length)
	setSwitch([{ id, status: el.checked }])
}

const changeAll = (currentChennels) => {
	if (currentChennels === totalChannels) {
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
			if (!arrSwitchOn.includes(i + 1) && i + 1 <= totalChannels) {
				arrSwitchOn.push(i + 1)
			}
			sw.checked = true
		})
	} else {
		allSwitch.forEach((sw, i) => {
			const index = arrSwitchOn.indexOf(i + 1)
			if (index > -1) {
				arrSwitchOn.splice(index, 1)
			}
			sw.checked = false
		})
	}
	setSwitch(getChannelStatus())
}

const getChannelStatus = () => {
	const arrChannelStatus = []
	for (let i = 1; i <= totalChannels; i++) {
		arrChannelStatus.push({
			id: i,
			status: arrSwitchOn.includes(i),
		})
	}
	return arrChannelStatus
}

const getChannel = async () => {
	try {
		ws.on("connected", () => {
			ws.send("get-room", {})
			ws.on("room-data", (data) => {
				data = data?.room
				if (data?.channels?.length >= 0) {
					totalChannels = data.channels.length

					arrSwitchOn.length = 0
					arrSwitchOn.push(
						...data.channels.filter((ch) => ch.status).map((ch) => ch.id)
					)

					changeAll(arrSwitchOn.length)
				}

				createChannel(data?.channels)
			})
		})
	} catch (error) {
		showToast(" Failed to get datay", true)
	}
}

const setSwitch = async (stateSwitch) => {
	try {
		const room = getIdUrlParams()
		ws.send("switch-channel", { room, channels: stateSwitch })
	} catch (error) {
		console.log(error)
		showToast(" Failed to set datay", true)
	}
}

document.addEventListener("DOMContentLoaded", () => {
	setTitle()
	getChannel()
})
