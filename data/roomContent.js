const createChannel = () => {
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
                  <input type="checkbox">
                  <span class="slider"></span>
                </label>
              </div>
            </div>
          </div>
		`
	}
}

document.addEventListener("DOMContentLoaded", () => {
	createChannel()
})
