const buttonEye = document.querySelector(".input_icon_eye")

buttonEye.addEventListener("click", () => {
	const inputPassword = document.querySelector("#wifi_password")
	inputPassword.type = inputPassword.type === "password" ? "text" : "password"
	buttonEye.src =
		inputPassword.type === "password"
			? "../asset/eye-off.svg"
			: "../asset/eye-show.svg"
})
