function goToRoom(element) {
	const roomNumber = element.dataset.room // ambil dari data-room
	window.location.href = `room.html?room=${roomNumber}`
}

function goToSetting() {
	window.location.href = `setting.html`
}
