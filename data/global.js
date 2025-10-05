const showToast = (message, error = false, duration = 3000) => {
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

// ws

class EventEmitter {
	constructor() {
		this.events = {}
	}

	on(event, listener) {
		if (!this.events[event]) this.events[event] = []
		this.events[event].push(listener)
	}

	off(event, listener) {
		if (!this.events[event]) return
		this.events[event] = this.events[event].filter((l) => l !== listener)
	}

	emit(event, data) {
		if (!this.events[event]) return
		this.events[event].forEach((listener) => listener(data))
	}
}

class WSClient extends EventEmitter {
	constructor(url) {
		super()
		this.socket = new WebSocket(url)

		this.socket.onopen = () => {
			console.log("Connected to WebSocket server")
			this.emit("connected")
		}

		this.socket.onmessage = (event) => {
			try {
				const msg = JSON.parse(event.data)
				if (msg.event) {
					this.emit(msg.event, msg.data)
				} else {
					this.emit("raw", msg)
				}
			} catch (e) {
				console.error("Invalid message", event.data)
			}
		}

		this.socket.onclose = () => {
			console.log("Disconnected")
			this.emit("disconnected")
		}
	}

	send(event, data) {
		const payload = JSON.stringify({ event, data })
		this.socket.send(payload)
	}
}

const ws = new WSClient(`ws://${window.location.host}/ws`)
// const ws = new WSClient("ws://localhost:8080")
