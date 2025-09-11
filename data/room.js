const setTitle = () => {
	const urlParams = new URLSearchParams(window.location.search)
	const room = urlParams.get("room")
	const titleRoom = room ? `Room ${room}` : "Room"
	document.querySelector(".title").innerText = titleRoom
}

document.addEventListener("DOMContentLoaded", () => {
	setTitle()
})
