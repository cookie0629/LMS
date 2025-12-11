LMSTranscodingMode = {
	Never: 0,
	Always: 1,
	IfFormatNotSupported: 2,
}
Object.freeze(LMSTranscodingMode); /** * 冻结LMSTranscodingMode对象，防止其被修改 * 冻结后的对象不能添加新属性，不能删除现有属性，不能修改现有属性的可枚举性、可配置性、可写性，也不能修改现有属性的值 * 也不能修改其原型 */

// LMSMediaPlayer 是一个媒体播放器类，用于处理音频播放功能，支持转码和文件两种模式，并提供音量控制、播放进度调整、设置保存等功能。
// 参数：
// root: 播放器根元素
// defaultSettings: 默认设置对象
// params: 音频轨道参数
// settings: 播放器设置对象
class LMSMediaPlayer {
	static #volumeStepAmount = 0.05;

	static #seekAmount = 5;

	static #Mode = {
		Transcoding: 1,
		File: 2,
	}

	#root;
	#elems;
	#offset;
	#trackId;
	#duration;
	#audioNativeSrc;
	#audioTranscodingSrc;
	#settings;
	#playedDuration;
	#lastStartPlaying;
	#audioIsInit;
	#pendingTrackParameters;
	#gainNode;
	#audioCtx;
	/**
	这是一个音频播放器的构造函数，用于初始化音频播放器的核心组件和事件监听器。它设置音频元素、播放/暂停按钮，并添加键盘和触摸事件监听器以控制播放器功能。
	参数：
	root：播放器的根元素
	defaultSettings：播放器的默认设置对象}
	*/
	constructor(root, defaultSettings) {
		this.#root = root;
		this.#elems = {};
		this.#offset = 0;
		this.#trackId = null;
		this.#duration = 0;
		this.#settings = {};
		this.#playedDuration = 0;
		this.#lastStartPlaying = null;
		this.#audioIsInit = false;
		this.#pendingTrackParameters = null;
		this.#gainNode = null;
		this.#audioCtx = null;

		this.#elems.audio = document.getElementById("lms-mp-audio");
		this.#elems.playpause = document.getElementById("lms-mp-playpause");
		this.#elems.previous = document.getElementById("lms-mp-previous");
		this.#elems.next = document.getElementById("lms-mp-next");
		this.#elems.progress = document.getElementById("lms-mp-progress");
		this.#elems.seek = document.getElementById("lms-mp-seek");
		this.#elems.curtime = document.getElementById("lms-mp-curtime");
		this.#elems.duration = document.getElementById("lms-mp-duration");
		this.#elems.volume = document.getElementById("lms-mp-volume");
		this.#elems.volumeslider = document.getElementById("lms-mp-volume-slider");
		this.#elems.transcodingActive = document.getElementById("lms-transcoding-active");

		this.#elems.playpause.addEventListener("click", () => {
			this.#playPause();
		});

		this.#elems.previous.addEventListener("click", () => {
			this.#playPrevious();
		});
		this.#elems.next.addEventListener("click", () => {
			this.#playNext();
		});
		this.#elems.seek.addEventListener("change", () => {
			this.#seekTo(parseInt(this.#elems.seek.value, 10));
		});

		this.#elems.audio.addEventListener("play", this.#updateControls.bind(this));
		this.#elems.audio.addEventListener("playing", this.#updateControls.bind(this));
		this.#elems.audio.addEventListener("pause", this.#updateControls.bind(this));

		this.#elems.audio.addEventListener("play", this.#updateMediaSessionState.bind(this));
		this.#elems.audio.addEventListener("playing", this.#updateMediaSessionState.bind(this));
		this.#elems.audio.addEventListener("pause", this.#updateMediaSessionState.bind(this));

		this.#elems.audio.addEventListener("pause", this.#pauseTimer.bind(this));
		this.#elems.audio.addEventListener("playing", this.#startTimer.bind(this));
		this.#elems.audio.addEventListener("waiting", this.#pauseTimer.bind(this));

		this.#elems.audio.addEventListener("timeupdate", () => {
			this.#elems.progress.style.width = "" + ((this.#offset + this.#elems.audio.currentTime) / this.#duration) * 100 + "%";
			this.#elems.curtime.innerHTML = this.#durationToString(this.#offset + this.#elems.audio.currentTime);
		});

		this.#elems.audio.addEventListener("ended", () => {
			this.#resetTimer();
			Wt.emit(this.#root, "playbackEnded");
		});

		this.#elems.audio.addEventListener("canplay", () => {
			if (this.#getAudioMode() == LMSMediaPlayer.#Mode.Transcoding) {
				this.#elems.transcodingActive.style.display = "inline";
			}
			else {
				this.#elems.transcodingActive.style.display = "none";
			}
		});

		this.#initVolume();
		this.#initDefaultSettings(defaultSettings);

		this.#elems.volumeslider.addEventListener("input", () => {
			this.#setVolume(this.#elems.volumeslider.value);
		});

		this.#elems.volume.addEventListener("click", () => {
			if (this.#elems.audio.volume != 0) {
				this.#setVolume(0);
			}
			else {
				this.#setVolume(this.#elems.lastvolume);
			}
		});

		document.addEventListener("keydown", (event) => { //  为文档添加键盘按下事件监听器 用于处理全局键盘快捷键
			let handled = false;

			if (event.target instanceof HTMLInputElement) //  如果事件目标是输入框，则不处理任何快捷键
				return;

			if (event.keyCode == 32) {
				this.#playPause();
				handled = true;
			}
			else if (event.ctrlKey && !event.shiftKey && event.keyCode == 37) {
				this.#playPrevious();
				handled = true;
			}
			else if (event.ctrlKey && !event.shiftKey && event.keyCode == 39) {
				this.#playNext();
				handled = true;
			}
			else if (event.ctrlKey && event.keyCode == 40) {
				this.#stepVolumeDown();
				handled = true;
			}
			else if (event.ctrlKey && event.keyCode == 38) {
				this.#stepVolumeUp();
				handled = true;
			}
			else if (event.ctrlKey && event.shiftKey && event.keyCode == 37) {
				this.#seekBack();
				handled = true;
			}
			else if (event.ctrlKey && event.shiftKey && event.keyCode == 39) {
				this.#seekForward();
				handled = true;
			}

			if (handled)
				event.preventDefault();
		});

		document.addEventListener("touchstart", this.#unlock.bind(this));
		document.addEventListener("touchend", this.#unlock.bind(this));
		document.addEventListener("click", this.#unlock.bind(this));
	}

	/**
	 * 该方法用于解锁音频上下文，通过移除所有事件监听器（touchstart、touchend 和 click）来解除锁定状态，并初始化音频上下文
	 */
	#unlock() {
		document.removeEventListener("touchstart", this.#unlock.bind(this));
		document.removeEventListener("touchend", this.#unlock.bind(this));
		document.removeEventListener("click", this.#unlock.bind(this));
		this.#initAudioCtx();
	};

	/**
	 初始化音频上下文，设置音频处理节点，并配置媒体会话控制功能。
	 如果音频上下文已经初始化，则直接返回。否则，创建音频上下文和增益节点，连接音频源。
	 如果支持媒体会话API，则设置播放、暂停、上一首、下一首和跳转等控制功能。
	 如果存在待处理的音频轨道参数，则应用这些参数。
	 */
	#initAudioCtx() {
		if (this.#audioIsInit)
			return;

		this.#audioIsInit = true;

		this.#audioCtx = new (window.AudioContext || window.webkitAudioContext)();
		this.#gainNode = this.#audioCtx.createGain();
		let source = this.#audioCtx.createMediaElementSource(this.#elems.audio);
		source.connect(this.#gainNode);
		this.#gainNode.connect(this.#audioCtx.destination);

		if ("mediaSession" in navigator) {
			navigator.mediaSession.setActionHandler("play", () => {
				this.#playPause();
			});
			navigator.mediaSession.setActionHandler("pause", () => {
				this.#playPause();
			});
			navigator.mediaSession.setActionHandler("previoustrack", () => {
				this.#playPrevious();
			});
			navigator.mediaSession.setActionHandler("nexttrack", () => {
				this.#playNext();
			});
			navigator.mediaSession.setActionHandler("seekto", (e) => {
				this.#seekTo(e.seekTime);
			});
		}

		if (this.#pendingTrackParameters != null) {
			this.#applyAudioTrackParameters(this.#pendingTrackParameters);
			this.#pendingTrackParameters = null;
		}
	}

	/**
	 如果存在待处理的音频轨道参数，则应用这些参数。
	 */
	#updateControls() {
		const pauseClass = "fa-pause";
		const playClass = "fa-play";

		if (this.#elems.audio.paused) {
			this.#elems.playpause.firstElementChild.classList.remove(pauseClass);
			this.#elems.playpause.firstElementChild.classList.add(playClass);
		}
		else {
			this.#elems.playpause.firstElementChild.classList.remove(playClass);
			this.#elems.playpause.firstElementChild.classList.add(pauseClass);
		}
	}

	/**
	 * 启动计时器，记录当前时间，如果是首次播放则触发"scrobbleListenNow"事件
	 */
	#startTimer() {
		if (this.#lastStartPlaying == null)
			Wt.emit(this.#root, "scrobbleListenNow", this.#trackId);
		this.#lastStartPlaying = Date.now();
	}

	/**
	 * 暂停计时器，计算已播放时间并重置计时器
	 */
	#pauseTimer() {
		if (this.#lastStartPlaying != null) {
			this.#playedDuration += Date.now() - this.#lastStartPlaying;
			this.#lastStartPlaying = null;
		}
	}
	/**
	 * 重置计时器，如果已播放时间大于0则触发"scrobbleListenFinished"事件，并重置已播放时间为0
	 */
	#resetTimer() {
		if (this.#lastStartPlaying != null)
			this.#pauseTimer();

		if (this.#playedDuration > 0) {
			Wt.emit(this.#root, "scrobbleListenFinished", this.#trackId, this.#playedDuration);
			this.#playedDuration = 0;
		}
	}

	/**
	将一个表示持续时间的数字转换为易读的时间字符串格式，格式为 HH:MM:SS，其中小时是可选的。
	 */
	#durationToString(duration) {
		const seconds = parseInt(duration, 10);
		const h = Math.floor(seconds / 3600);
		const m = Math.floor((seconds % 3600) / 60);
		const s = Math.round(seconds % 60);
		return [
			h,
			m > 9 ? m : (h ? '0' + m : m || '0'),
			s > 9 ? s : '0' + s
		].filter(Boolean).join(':');
	}

	// 播放音频
	#playTrack() {
		this.#elems.audio.play()
			.then(_ => { })
			.catch(error => { console.log("Cannot play audio: " + error); });
	}

	// 控制音频的播放和暂停功能。当音频处于暂停状态且有音轨时，则播放音轨；否则暂停音频播放。
	#playPause() {
		this.#initAudioCtx();

		if (this.#audioCtx.state === "suspended") {
			this.#audioCtx.resume().catch(err => console.warn(err));
		}

		if (this.#elems.audio.paused && this.#elems.audio.children.length > 0) {
			this.#playTrack();
		}
		else
			this.#elems.audio.pause();
	}

	// 播放上一个媒体项目。
	#playPrevious() {
		this.#initAudioCtx();
		Wt.emit(this.#root, "playPrevious");
	}

	// 触发播放下一个音频的功能。
	#playNext() {
		this.#initAudioCtx();
		Wt.emit(this.#root, "playNext");
	}

	// 初始化音量设置，从本地存储中读取保存的音量值并应用到音量滑块，然后设置当前音量。
	#initVolume() {
		if (typeof (Storage) !== "undefined" && localStorage.volume) {
			this.#elems.volumeslider.value = Number(localStorage.volume);
		}

		this.#setVolume(this.#elems.volumeslider.value);
	}

	#initDefaultSettings = function (defaultSettings) {
		if (typeof (Storage) !== "undefined" && localStorage.settings) {
			this.#settings = Object.assign(defaultSettings, JSON.parse(localStorage.settings));
		}
		else {
			this.#settings = defaultSettings;
		}

		Wt.emit(this.#root, "settingsLoaded", JSON.stringify(this.#settings));
	}

	//设置音频音量，并更新音量滑块和音量图标的显示状态，同时将音量值保存到本地存储。
	#setVolume(volume) {
		this.#elems.lastvolume = this.#elems.audio.volume;

		this.#elems.audio.volume = volume;
		this.#elems.volumeslider.value = volume;

		if (volume > 0.5) {
			this.#elems.volume.classList.remove("fa-volume-off");
			this.#elems.volume.classList.remove("fa-volume-down");
			this.#elems.volume.classList.add("fa-volume-up");
		}
		else if (volume > 0) {
			this.#elems.volume.classList.remove("fa-volume-off");
			this.#elems.volume.classList.remove("fa-volume-up");
			this.#elems.volume.classList.add("fa-volume-down");
		}
		else {
			this.#elems.volume.classList.remove("fa-volume-up");
			this.#elems.volume.classList.remove("fa-volume-down");
			this.#elems.volume.classList.add("fa-volume-off");
		}

		if (typeof (Storage) !== "undefined") {
			localStorage.volume = volume;
		}
	}

	/**
	 * 该方法用于减小音频音量，按照设定的音量步长进行递减。
	 * 如果当前音量不是步长的整数倍，则递减到最接近的较低步长值。音量最小不会低于0。
	 */
	#stepVolumeDown() {
		let currentVolume = this.#elems.audio.volume;
		let remainder = (currentVolume * 10) % (LMSMediaPlayer.#volumeStepAmount * 10);
		let newVolume = remainder === 0 ? currentVolume - LMSMediaPlayer.#volumeStepAmount : currentVolume - (remainder / 10);
		this.#setVolume(Math.max(newVolume, 0));
	}

	// 用于将音量按固定步长增加，如果当前音量不是步长的整数倍，则调整到下一个步长。
	#stepVolumeUp() {
		let currentVolume = this.#elems.audio.volume;
		let remainder = (currentVolume * 10) % (LMSMediaPlayer.#volumeStepAmount * 10);
		let newVolume = remainder === 0 ? currentVolume + LMSMediaPlayer.#volumeStepAmount : currentVolume + (LMSMediaPlayer.#volumeStepAmount - (remainder / 10));
		this.#setVolume(Math.min(newVolume, 1));
	}

	// 设置音频的重播增益值，通过计算并调整增益节点的增益值来实现
	#setReplayGain(replayGain) {
		this.#gainNode.gain.value = Math.pow(10, (this.#settings.replayGain.preAmpGain + replayGain) / 20);
	}

	/**
	该功能用于将音频播放器的播放位置跳转到指定的时间点。
	根据当前的音频模式（转码模式或文件模式）采取不同的跳转策略。
	如果音频原本正在播放，跳转后会继续播放。
	 */
	#seekTo(seekTime) {
		this.#initAudioCtx();
		let mode = this.#getAudioMode();
		if (!mode)
			return;

		let wasPlaying = !this.#elems.audio.paused;

		switch (mode) {
			case LMSMediaPlayer.#Mode.Transcoding:
				this.#offset = seekTime;
				this.#removeAudioSources();
				this.#addAudioSource(this.#audioTranscodingSrc + "&offset=" + this.#offset);
				this.#elems.audio.load();
				this.#elems.audio.currentTime = 0;
				break;

			case LMSMediaPlayer.#Mode.File:
				this.#elems.audio.currentTime = seekTime;
				break;
		}

		if (wasPlaying)
			this.#playTrack();

		this.#updateMediaSessionState();
	}

	// 该方法用于将音频播放位置向后回退指定的时间量
	#seekBack() {
		let currentPosition = this.#offset + this.#elems.audio.currentTime;
		let newPosition = currentPosition - LMSMediaPlayer.#seekAmount;
		this.#seekTo(Math.max(newPosition, 0));
	}

	#seekForward() {
		let currentPosition = this.#offset + this.#elems.audio.currentTime;
		let newPosition = currentPosition + LMSMediaPlayer.#seekAmount;
		this.#seekTo(Math.min(newPosition, this.#duration));
	}

	// 更新媒体会话(Media Session)的状态，包括播放进度、播放状态和持续时间信息
	#updateMediaSessionState() {
		if ("mediaSession" in navigator) {
			navigator.mediaSession.setPositionState({
				duration: this.#duration,
				playbackRate: 1,
				position: Math.min(this.#offset + this.#elems.audio.currentTime, this.#duration),
			});

			if (this.#elems.audio.paused)
				navigator.mediaSession.playbackState = "paused";
			else
				navigator.mediaSession.playbackState = "playing";
		}
	}

	// 该方法用于移除音频元素中的所有子元素，以便清除之前的音频源。
	#removeAudioSources() {
		while (this.#elems.audio.lastElementChild) {
			this.#elems.audio.removeChild(this.#elems.audio.lastElementChild);
		}
	}

	// 该方法用于向音频元素中添加一个新的音频源，通过创建一个新的source元素并设置其src属性来实现。
	#addAudioSource(audioSrc) {
		let source = document.createElement('source');
		source.src = audioSrc;
		this.#elems.audio.appendChild(source);
	}

	// 该方法用于获取当前音频播放的模式，即是否处于转码模式或文件模式。
	#getAudioMode() {
		if (this.#elems.audio.currentSrc) {
			if (this.#elems.audio.currentSrc.includes("format"))
				return LMSMediaPlayer.#Mode.Transcoding;
			else
				return LMSMediaPlayer.#Mode.File;
		}
		else
			return undefined;
	}

	// 该方法用于应用音频轨道的参数，包括重播增益、媒体会话的元数据等。
	#applyAudioTrackParameters(params) {
		this.#setReplayGain(params.replayGain);
		if ("mediaSession" in navigator) {
			navigator.mediaSession.metadata = new MediaMetadata({
				title: params.title,
				artist: params.artist,
				album: params.release,
				artwork: params.artwork,
			});
		}
	}

	// 该方法用于播放音频轨道，包括初始化音频上下文、加载音频轨道、设置播放状态等。
	loadTrack(params, autoplay) {
		this.#resetTimer();

		this.#trackId = params.trackId;
		this.#offset = 0;
		this.#duration = params.duration;
		this.#audioNativeSrc = params.nativeResource;
		this.#audioTranscodingSrc = params.transcodingResource + "&bitrate=" + this.#settings.transcoding.bitrate + "&format=" + this.#settings.transcoding.format;

		this.#elems.seek.max = this.#duration;

		this.#removeAudioSources();
		if (this.#settings.transcoding.mode == LMSTranscodingMode.Never || this.#settings.transcoding.mode == LMSTranscodingMode.IfFormatNotSupported) {
			this.#addAudioSource(this.#audioNativeSrc);
		}
		if (this.#settings.transcoding.mode == LMSTranscodingMode.Always || this.#settings.transcoding.mode == LMSTranscodingMode.IfFormatNotSupported) {
			this.#addAudioSource(this.#audioTranscodingSrc);
		}
		this.#elems.audio.load();

		this.#elems.curtime.innerHTML = this.#durationToString(this.#offset);
		this.#elems.duration.innerHTML = this.#durationToString(this.#duration);

		if (!this.#audioIsInit) {
			this.#pendingTrackParameters = params;
			return;
		}

		this.#applyAudioTrackParameters(params);

		if (autoplay && this.#audioCtx.state == "running")
			this.#playTrack();
	}

	// 暂停音频播放。
	stop() {
		this.#elems.audio.pause();
	}

	// 此方法用于设置实例的配置，并将配置保存到浏览器的本地存储中（如果支持
	setSettings(settings) {
		this.#settings = settings;

		if (typeof (Storage) !== "undefined") {
			localStorage.settings = JSON.stringify(this.#settings);
		}
	}
}

// @license-end
