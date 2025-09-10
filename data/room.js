const urlParams = new URLSearchParams(window.location.search)
const room = urlParams.get("room")

console.log(room)
