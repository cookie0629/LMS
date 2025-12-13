/**
 * 音乐主题炫酷交互效果 JavaScript
 * 包含页面过渡、鼠标交互、滚动动画等功能
 */

(function() {
	'use strict';

	// ========== 页面加载动画 ==========
	document.addEventListener('DOMContentLoaded', function() {
		initPageTransitions();
		initMouseInteractions();
		initScrollAnimations();
		initAudioWaveform();
		initVinylRecordEffect();
		initParticleEffect();
		initNeonGlowEffect();
	});

	// ========== 页面过渡动画 ==========
	function initPageTransitions() {
		// 为容器添加淡入动画
		const containers = document.querySelectorAll('.container');
		containers.forEach((container, index) => {
			container.style.opacity = '0';
			container.style.transform = 'translateY(20px)';
			container.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
			
			setTimeout(() => {
				container.style.opacity = '1';
				container.style.transform = 'translateY(0)';
			}, index * 100);
		});

		// 为列表项添加交错动画
		const listItems = document.querySelectorAll('.Lms-row-container > *');
		listItems.forEach((item, index) => {
			item.style.opacity = '0';
			item.style.transform = 'translateX(-20px)';
			item.style.transition = `opacity 0.4s ease ${index * 0.05}s, transform 0.4s ease ${index * 0.05}s`;
			
			setTimeout(() => {
				item.style.opacity = '1';
				item.style.transform = 'translateX(0)';
			}, 100 + index * 50);
		});

		// 为卡片添加淡入动画
		const cards = document.querySelectorAll('.Lms-bg-dark-hover');
		cards.forEach((card, index) => {
			card.style.opacity = '0';
			card.style.transform = 'scale(0.9)';
			card.style.transition = `opacity 0.5s ease ${index * 0.1}s, transform 0.5s ease ${index * 0.1}s`;
			
			setTimeout(() => {
				card.style.opacity = '1';
				card.style.transform = 'scale(1)';
			}, 200 + index * 100);
		});
	}

	// ========== 鼠标交互效果 ==========
	function initMouseInteractions() {
		// 鼠标跟随光效
		let mouseX = 0;
		let mouseY = 0;
		let cursorGlow = null;

		// 创建鼠标光效元素
		function createCursorGlow() {
			cursorGlow = document.createElement('div');
			cursorGlow.style.cssText = `
				position: fixed;
				width: 300px;
				height: 300px;
				border-radius: 50%;
				background: radial-gradient(circle, rgba(255, 0, 110, 0.2) 0%, transparent 70%);
				pointer-events: none;
				z-index: 9999;
				transform: translate(-50%, -50%);
				transition: opacity 0.3s ease;
				opacity: 0;
			`;
			document.body.appendChild(cursorGlow);
		}

		createCursorGlow();

		// 鼠标移动事件
		document.addEventListener('mousemove', function(e) {
			mouseX = e.clientX;
			mouseY = e.clientY;
			
			if (cursorGlow) {
				cursorGlow.style.left = mouseX + 'px';
				cursorGlow.style.top = mouseY + 'px';
				cursorGlow.style.opacity = '1';
			}
		});

		// 鼠标离开窗口时隐藏光效
		document.addEventListener('mouseleave', function() {
			if (cursorGlow) {
				cursorGlow.style.opacity = '0';
			}
		});

		// 按钮点击波纹效果
		const buttons = document.querySelectorAll('.btn, .nav-link, .Lms-badge-cluster');
		buttons.forEach(button => {
			button.addEventListener('click', function(e) {
				const ripple = document.createElement('span');
				const rect = this.getBoundingClientRect();
				const size = Math.max(rect.width, rect.height);
				const x = e.clientX - rect.left - size / 2;
				const y = e.clientY - rect.top - size / 2;

				ripple.style.cssText = `
					position: absolute;
					width: ${size}px;
					height: ${size}px;
					border-radius: 50%;
					background: rgba(255, 0, 110, 0.4);
					left: ${x}px;
					top: ${y}px;
					transform: scale(0);
					animation: ripple 0.6s ease-out;
					pointer-events: none;
				`;

				this.style.position = 'relative';
				this.style.overflow = 'hidden';
				this.appendChild(ripple);

				setTimeout(() => ripple.remove(), 600);
			});
		});

		// 添加波纹动画样式
		if (!document.getElementById('ripple-animation-style')) {
			const style = document.createElement('style');
			style.id = 'ripple-animation-style';
			style.textContent = `
				@keyframes ripple {
					to {
						transform: scale(2);
						opacity: 0;
					}
				}
			`;
			document.head.appendChild(style);
		}

		// 封面图片鼠标跟随倾斜效果
		const covers = document.querySelectorAll('.Lms-cover');
		covers.forEach(cover => {
			cover.addEventListener('mousemove', function(e) {
				const rect = this.getBoundingClientRect();
				const x = e.clientX - rect.left;
				const y = e.clientY - rect.top;
				const centerX = rect.width / 2;
				const centerY = rect.height / 2;
				const rotateX = (y - centerY) / 10;
				const rotateY = (centerX - x) / 10;

				this.style.transform = `perspective(1000px) rotateX(${rotateX}deg) rotateY(${rotateY}deg) scale(1.05)`;
			});

			cover.addEventListener('mouseleave', function() {
				this.style.transform = 'perspective(1000px) rotateX(0) rotateY(0) scale(1)';
			});
		});
	}

	// ========== 滚动动画效果 ==========
	function initScrollAnimations() {
		const observerOptions = {
			threshold: 0.1,
			rootMargin: '0px 0px -50px 0px'
		};

		const observer = new IntersectionObserver(function(entries) {
			entries.forEach(entry => {
				if (entry.isIntersecting) {
					entry.target.style.opacity = '1';
					entry.target.style.transform = 'translateY(0)';
					observer.unobserve(entry.target);
				}
			});
		}, observerOptions);

		// 观察所有需要动画的元素
		const animatedElements = document.querySelectorAll(
			'.Lms-row-container > *, .Lms-bg-dark-hover, .card, .modal-content'
		);

		animatedElements.forEach((el, index) => {
			el.style.opacity = '0';
			el.style.transform = 'translateY(30px)';
			el.style.transition = `opacity 0.6s ease ${index * 0.05}s, transform 0.6s ease ${index * 0.05}s`;
			observer.observe(el);
		});

		// 滚动时导航栏效果
		let lastScroll = 0;
		const navbar = document.querySelector('.navbar');
		
		window.addEventListener('scroll', function() {
			const currentScroll = window.pageYOffset;
			
			if (navbar) {
				if (currentScroll > lastScroll && currentScroll > 100) {
					navbar.style.transform = 'translateY(-100%)';
					navbar.style.transition = 'transform 0.3s ease';
				} else {
					navbar.style.transform = 'translateY(0)';
				}
			}
			
			lastScroll = currentScroll;
		});

		// 视差滚动效果
		const parallaxElements = document.querySelectorAll('.Lms-cover-container');
		window.addEventListener('scroll', function() {
			const scrolled = window.pageYOffset;
			parallaxElements.forEach((el, index) => {
				const rate = scrolled * 0.1 * (index % 2 === 0 ? 1 : -1);
				el.style.transform = `translateY(${rate}px)`;
			});
		});
	}

	// ========== 音频波形可视化 ==========
	function initAudioWaveform() {
		// 在播放器中添加波形可视化
		const player = document.querySelector('.Lms-player');
		if (player && !document.querySelector('.music-waveform')) {
			const waveform = document.createElement('div');
			waveform.className = 'music-waveform';
			waveform.style.cssText = 'position: absolute; right: 20px; top: 50%; transform: translateY(-50%);';
			
			for (let i = 0; i < 8; i++) {
				const bar = document.createElement('div');
				bar.className = 'wave-bar';
				waveform.appendChild(bar);
			}
			
			// 尝试插入到播放器控制区域
			const playerControls = player.querySelector('.d-flex.align-items-center');
			if (playerControls) {
				playerControls.style.position = 'relative';
				playerControls.appendChild(waveform);
			}
		}

		// 根据音频播放状态控制波形动画
		const audio = document.getElementById('lms-mp-audio');
		if (audio) {
			const waveform = document.querySelector('.music-waveform');
			
			audio.addEventListener('play', function() {
				if (waveform) {
					waveform.style.opacity = '1';
					const bars = waveform.querySelectorAll('.wave-bar');
					bars.forEach(bar => {
						bar.style.animationPlayState = 'running';
					});
				}
			});

			audio.addEventListener('pause', function() {
				if (waveform) {
					const bars = waveform.querySelectorAll('.wave-bar');
					bars.forEach(bar => {
						bar.style.animationPlayState = 'paused';
						bar.style.height = '20%';
					});
				}
			});
		}
	}

	// ========== 黑胶唱片旋转效果 ==========
	function initVinylRecordEffect() {
		const covers = document.querySelectorAll('.Lms-cover');
		
		covers.forEach(cover => {
			// 检查是否正在播放
			const checkPlaying = () => {
				const entry = cover.closest('.Lms-entry-playing, .Lms-row-container > *');
				if (entry && entry.classList.contains('Lms-entry-playing')) {
					cover.style.animation = 'vinylSpin 3s linear infinite';
				} else {
					cover.style.animation = 'none';
				}
			};

			// 初始检查
			checkPlaying();

			// 监听播放状态变化
			const observer = new MutationObserver(checkPlaying);
			const entry = cover.closest('.Lms-row-container > *');
			if (entry) {
				observer.observe(entry, {
					attributes: true,
					attributeFilter: ['class']
				});
			}
		});

		// 添加旋转动画样式
		if (!document.getElementById('vinyl-spin-style')) {
			const style = document.createElement('style');
			style.id = 'vinyl-spin-style';
			style.textContent = `
				@keyframes vinylSpin {
					from {
						transform: rotate(0deg);
					}
					to {
						transform: rotate(360deg);
					}
				}
			`;
			document.head.appendChild(style);
		}
	}

	// ========== 粒子效果 ==========
	function initParticleEffect() {
		// 在鼠标点击时创建粒子
		document.addEventListener('click', function(e) {
			// 只在特定元素上触发
			if (e.target.closest('.btn, .Lms-cover-anchor, .nav-link')) {
				createParticles(e.clientX, e.clientY);
			}
		});

		function createParticles(x, y) {
			const colors = ['#ff006e', '#8338ec', '#3a86ff', '#06ffa5'];
			
			for (let i = 0; i < 8; i++) {
				const particle = document.createElement('div');
				const color = colors[Math.floor(Math.random() * colors.length)];
				const angle = (Math.PI * 2 * i) / 8;
				const velocity = 50 + Math.random() * 50;
				
				particle.style.cssText = `
					position: fixed;
					left: ${x}px;
					top: ${y}px;
					width: 6px;
					height: 6px;
					background: ${color};
					border-radius: 50%;
					pointer-events: none;
					z-index: 10000;
					box-shadow: 0 0 10px ${color};
				`;

				document.body.appendChild(particle);

				const vx = Math.cos(angle) * velocity;
				const vy = Math.sin(angle) * velocity;

				let posX = x;
				let posY = y;
				let opacity = 1;

				function animate() {
					posX += vx * 0.1;
					posY += vy * 0.1;
					opacity -= 0.02;

					particle.style.left = posX + 'px';
					particle.style.top = posY + 'px';
					particle.style.opacity = opacity;

					if (opacity > 0) {
						requestAnimationFrame(animate);
					} else {
						particle.remove();
					}
				}

				requestAnimationFrame(animate);
			}
		}
	}

	// ========== 霓虹灯光效果 ==========
	function initNeonGlowEffect() {
		// 为按钮添加霓虹光效
		const buttons = document.querySelectorAll('.btn-primary, .btn-outline-primary');
		buttons.forEach(button => {
			button.addEventListener('mouseenter', function() {
				this.style.textShadow = '0 0 10px currentColor, 0 0 20px currentColor, 0 0 30px currentColor';
			});

			button.addEventListener('mouseleave', function() {
				this.style.textShadow = 'none';
			});
		});

		// 为链接添加霓虹光效
		const links = document.querySelectorAll('.link-success, .link-secondary, .nav-link');
		links.forEach(link => {
			link.addEventListener('mouseenter', function() {
				this.style.textShadow = '0 0 8px currentColor';
			});

			link.addEventListener('mouseleave', function() {
				this.style.textShadow = 'none';
			});
		});

		// 播放器标题霓虹效果
		const playerTitle = document.querySelector('#lms-mp-playpause');
		if (playerTitle) {
			const audio = document.getElementById('lms-mp-audio');
			if (audio) {
				audio.addEventListener('play', function() {
					playerTitle.style.boxShadow = '0 0 20px rgba(255, 0, 110, 0.8)';
				});

				audio.addEventListener('pause', function() {
					playerTitle.style.boxShadow = 'none';
				});
			}
		}
	}

	// ========== 音乐律动效果 ==========
	function initMusicRhythm() {
		// 监听音频播放，创建律动效果
		const audio = document.getElementById('lms-mp-audio');
		if (!audio) return;

		let audioContext = null;
		let analyser = null;
		let dataArray = null;

		audio.addEventListener('play', function() {
			try {
				if (!audioContext) {
					audioContext = new (window.AudioContext || window.webkitAudioContext)();
					analyser = audioContext.createAnalyser();
					const source = audioContext.createMediaElementSource(audio);
					source.connect(analyser);
					analyser.connect(audioContext.destination);
					analyser.fftSize = 256;
					dataArray = new Uint8Array(analyser.frequencyBinCount);
				}

				function updateRhythm() {
					if (analyser && dataArray && !audio.paused) {
						analyser.getByteFrequencyData(dataArray);
						
						// 获取低频能量
						const bassEnergy = dataArray.slice(0, 10).reduce((a, b) => a + b, 0) / 10;
						
						// 应用到背景
						document.body.style.filter = `brightness(${100 + bassEnergy / 10}%)`;
						
						// 应用到播放器
						const player = document.querySelector('.Lms-player');
						if (player) {
							const intensity = bassEnergy / 255;
							player.style.boxShadow = `0 -4px ${20 + intensity * 20}px rgba(255, 0, 110, ${0.4 + intensity * 0.3})`;
						}

						requestAnimationFrame(updateRhythm);
					}
				}

				updateRhythm();
			} catch (e) {
				console.log('音频分析器初始化失败:', e);
			}
		});
	}

	// 初始化音乐律动（延迟执行，等待音频元素完全加载）
	setTimeout(initMusicRhythm, 1000);

	// ========== 页面可见性变化时的效果 ==========
	document.addEventListener('visibilitychange', function() {
		if (document.hidden) {
			// 页面隐藏时暂停动画
			document.body.style.animationPlayState = 'paused';
		} else {
			// 页面显示时恢复动画
			document.body.style.animationPlayState = 'running';
		}
	});

})();

