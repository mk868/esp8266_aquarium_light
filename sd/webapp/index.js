Vue.component('level-input', {
	props: ['max', 'value'],
	template: '<div>' + 
			'<input type="range" :value="value" class="form-control" min="0" :max="max" @input="onInput" @change="onChange"/>' + 
			'<br/>' + 
			'<span>{{ ((value) / max * 100).toFixed() }}%</span>' +
			'</div>',
	methods: {
		onInput: function(event){
			this.$emit('update:value', event.target.value);
		},
		onChange: function(event){
			this.$emit('change', event.target.value);
		}
	}
});

Vue.component('module-enabled', {
	props: ['enabled', 'text'],
	template: '<h2>' + 
			'<label>' +
			'<input type="checkbox" :checked="enabled" @change="onChange"/> {{ text }}' +
			'</label>' +
			'</h2>',
	methods: {
		onChange: function(event){
			this.$emit('update:enabled', event.target.checked);
			this.$emit('change', event.target.checked);
		}
	}
});

Vue.component('color-input', {
	props: ['color'],
	template: '<div>' +
			'<input type="color" :value="color" @input="onInput" @change="onChange" class="form-control"/>' +
			'<br/>' +
			'<span>{{ color }}</span>' +
			'</div>',
	methods: {
		onInput: function(event){
			this.$emit('update:color', event.target.value);
		},
		onChange: function(event){
			this.$emit('change', event.target.value);
		}
	}
});

var app = new Vue({ 
    el: '#app',
    data: {
		//day-mode
		day_L0_enabled: false,
        day_L0_level: 0,
		day_L1_enabled: false,
		day_L1_color: '#000000',
		
		//night-mode
		night_L0_enabled: false,
        night_L0_level: 0,
		night_L1_enabled: false,
		night_L1_color: '#000000',
		
		//settings
		night_mode_enabled: 0,
		night_mode_start: '01:00',
		night_mode_end: '02:00',
		timezone: '0:00',
		
		sendingData: 0,
		tab: 'day-mode'
    },
	methods: {
		day_L0_save: function (event) {
			this.sendingData++;
			axios.post(API_DAY_L0, {
					enabled: this.day_L0_enabled,
					level: this.day_L0_level
				})
				.then(function (response) {
					app.sendingData--;
					console.log(response);
				})
				.catch(function (error) {
					app.sendingData--;
					console.log(error);
				});
		},
		day_L1_save: function (event) {
			this.sendingData++;
			var rgb = hexToRGB(this.day_L1_color);
			axios.post(API_DAY_L1, {
					enabled: this.day_L1_enabled,
					levelR: Math.round(rgb.r / 255 * LEVEL_MAX_VALUE),
					levelG: Math.round(rgb.g / 255 * LEVEL_MAX_VALUE),
					levelB: Math.round(rgb.b / 255 * LEVEL_MAX_VALUE)
				})
				.then(function (response) {
					app.sendingData--;
					console.log(response);
				})
				.catch(function (error) {
					app.sendingData--;
					console.log(error);
				});
		},
		night_L0_save: function (event) {
			this.sendingData++;
			axios.post(API_NIGHT_L0, {
					enabled: this.night_L0_enabled,
					level: this.night_L0_level
				})
				.then(function (response) {
					app.sendingData--;
					console.log(response);
				})
				.catch(function (error) {
					app.sendingData--;
					console.log(error);
				});
		},
		night_L1_save: function (event) {
			this.sendingData++;
			var rgb = hexToRGB(this.night_L1_color);
			axios.post(API_NIGHT_L1, {
					enabled: this.night_L1_enabled,
					levelR: Math.round(rgb.r / 255 * LEVEL_MAX_VALUE),
					levelG: Math.round(rgb.g / 255 * LEVEL_MAX_VALUE),
					levelB: Math.round(rgb.b / 255 * LEVEL_MAX_VALUE)
				})
				.then(function (response) {
					app.sendingData--;
					console.log(response);
				})
				.catch(function (error) {
					app.sendingData--;
					console.log(error);
				});
		},
		settings_save: function (event) {
			this.sendingData++;
			axios.post(API_SETTINGS, {
					night_mode_enabled: this.night_mode_enabled,
					night_mode_start: this.night_mode_start,
					night_mode_end: this.night_mode_end,
					timezone: this.timezone
				})
				.then(function (response) {
					app.sendingData--;
					console.log(response);
				})
				.catch(function (error) {
					app.sendingData--;
					console.log(error);
				});
		},
	}
});

axios.get(API_DAY_L0)
	.then(function (res){
		app.day_L0_enabled = res.data.enabled;
		app.day_L0_level = res.data.level;
	})
	.catch(function (error) {
		console.error(error);
	});

axios.get(API_DAY_L1)
	.then(function (res){
		app.day_L1_enabled = res.data.enabled;
		var rgb = rgbToHex(Math.round(res.data.levelR / LEVEL_MAX_VALUE * 255), Math.round(res.data.levelG / LEVEL_MAX_VALUE * 255), Math.round(res.data.levelB / LEVEL_MAX_VALUE * 255));
		rgb = rgb.toUpperCase();
		app.day_L1_color = rgb;
	})
	.catch(function (error) {
		console.error(error);
	});

axios.get(API_NIGHT_L0)
	.then(function (res){
		app.night_L0_enabled = res.data.enabled;
		app.night_L0_level = res.data.level;
	})
	.catch(function (error) {
		console.error(error);
	});

axios.get(API_NIGHT_L1)
	.then(function (res){
		app.night_L1_enabled = res.data.enabled;
		var rgb = rgbToHex(Math.round(res.data.levelR / LEVEL_MAX_VALUE * 255), Math.round(res.data.levelG / LEVEL_MAX_VALUE * 255), Math.round(res.data.levelB / LEVEL_MAX_VALUE * 255));
		rgb = rgb.toUpperCase();
		app.night_L1_color = rgb;
	})
	.catch(function (error) {
		console.error(error);
	});

axios.get(API_SETTINGS)
	.then(function (res){
		app.night_mode_enabled = res.data.night_mode_enabled;
		var night_mode_start = res.data.night_mode_start;
		var night_mode_end = res.data.night_mode_end;
		var timezone = res.data.timezone;

		if(night_mode_start.indexOf(':') < 2){
			night_mode_start = '0' + night_mode_start;
		}
		if(night_mode_end.indexOf(':') < 2){
			night_mode_end = '0' + night_mode_end;
		}
		
		app.night_mode_start = night_mode_start;
		app.night_mode_end = night_mode_end;
		app.timezone = timezone;
	})
	.catch(function (error) {
		console.error(error);
	});
