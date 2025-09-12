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
