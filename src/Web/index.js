/*!
 * Ext JS Library 3.4.0
 * Copyright(c) 2006-2011 Sencha Inc.
 * licensing@sencha.com
 * http://www.sencha.com/license
 */
Date.prototype.format = function(format) {
	/*
	 * eg:format="YYYY-MM-dd hh:mm:ss";
	 */
	var o = {
		"M+" : this.getMonth() + 1, //month
		"d+" : this.getDate(), //day
		"h+" : this.getHours(), //hour
		"m+" : this.getMinutes(), //minute
		"s+" : this.getSeconds(), //second
		"q+" : Math.floor((this.getMonth() + 3) / 3), //quarter
		"S" : this.getMilliseconds() //millisecond
	}

	if (/(y+)/.test(format)) {
		format = format.replace(RegExp.$1, (this.getFullYear() + "").substr(4 - RegExp.$1.length));
	}

	for (var k in o) {
		if (new RegExp("(" + k + ")").test(format)) {
			format = format.replace(RegExp.$1, RegExp.$1.length == 1 ? o[k] : ("00" + o[k]).substr(("" + o[k]).length));
		}
	}
	return format;
}
/**
 * desktop instance.
 */
MyDesktop = new Ext.app.App({
	init : function() {
		Ext.QuickTips.init();
	},

	getModules : function() {
		return [this.LWindow, new MyDesktop.UserLogWindow(), this.LMonWindow, new MyDesktop.ClientManagerWindow(), new MyDesktop.AccordionWindow(), new MyDesktop.BogusMenuModule(), new MyDesktop.BogusMenuModule()];
	},

	// config for the start menu
	getStartConfig : function() {
		return {
			title : 'Centny',
			iconCls : 'user',
			toolItems : [{
				text : 'Login',
				iconCls : 'user-green',
				scope : this,
				handler : function() {
					MyDesktop.Login();
				}
			}, '-', {
				text : 'Logout',
				iconCls : 'user-red',
				scope : this,
				handler : function() {
					Ext.Ajax.request({
						url : 'Logout',
						method : 'GET',
						success : function(response, options) {
							Ext.MessageBox.alert('Success', response.responseText);
						},
						failure : function(response, options) {
							Ext.MessageBox.alert('Failed', 'Server error code:' + response.status);
						}
					});
				}
			}]
		};
	}
});
/**
 * login.
 */
MyDesktop.Login = function() {
	if (!this.LWindow) {
		return;
	}
	this.LWindow.createWindow();
}
/*
 * user log window.
 */
MyDesktop.log = {
	lgary : new Array(),
	logContainer : null,
	add : function(item) {
		if (item == null || item == "") {
			return;
		}
		item = new Date().format("yyyy-MM-dd hh:mm:ss [Log]&nbsp;&nbsp;" + item);
		var ncount = this.lgary.push(item);
		if (this.changed)
			this.changed(item);
		return ncount;
	},
	allToContainer : function() {
		if (this.lgary == null || this.lgary.length < 1) {
			return;
		}
		for (item in this.lgary) {
			if (item == "remove") {
				continue;
			}
			this.changed(this.lgary[item]);
		}
	},
	changed : function(nitem) {
		//console.log(this.logContainer);
		if (this.logContainer) {
			this.logContainer.createChild({
				html : "<div>" + nitem + "</div>"
			});
		}
	}
}
/*
 * login window.
 */
MyDesktop.LoginWindow = Ext.extend(Ext.app.Module, {
	id : 'login-win',
	init : function() {
		this.launcher = {
			text : 'Login',
			iconCls : 'icon-grid',
			handler : this.createWindow,
			scope : this
		}
	},
	createWindow : function() {
		var desktop = this.app.getDesktop();
		var win = desktop.getWindow('login-win');
		if (!win) {
			var form = new Ext.form.FormPanel({
				baseCls : 'x-plain',
				layout : 'absolute',
				monitorValid : true,
				defaultType : 'textfield',
				defaults : {
					allowBlank : false
				},
				items : [{
					x : 0,
					y : 10,
					xtype : 'label',
					text : 'Username:'
				}, {
					x : 70,
					y : 6,
					name : 'usr',
					anchor : '98%'
				}, {
					x : 0,
					y : 40,
					xtype : 'label',
					text : 'Password:'
				}, {
					x : 70,
					y : 39,
					name : 'pwd',
					inputType : "password",
					anchor : '98%' // anchor width by percentage
				}],
				buttonAlign : 'center',
				buttons : [{
					text : 'Cancel',
					formBind : false,
					handler : function(btn, evt) {
						win && win.close();
					}
				}, {
					text : 'Login',
					formBind : true,
					handler : function(btn, evt) {
						var rform = form.getForm();
						if (rform.isValid()) {
							Ext.Ajax.request({
								url : 'Login',
								method : 'GET',
								params : rform.getValues(),
								success : function(response, options) {
									Ext.MessageBox.alert('Success', 'Login success');
									win && win.close();
								},
								failure : function(response, options) {
									Ext.MessageBox.alert('Failed', 'Invalid username or password');
								}
							});
						} else {
							Ext.Msg.alert('Failed', "invalid username or password");
						}
					}
				}]
			});
			win = new Ext.Window({
				id : 'login-win',
				title : 'Login',
				width : 250,
				height : 145,
				layout : 'fit',
				plain : true,
				bodyStyle : 'padding:5px;',
				modal : true,
				items : form
			});
		}
		win.show();
	}
});
MyDesktop.LWindow = new MyDesktop.LoginWindow();
/*
 * user log window.
 */
MyDesktop.UserLogWindow = Ext.extend(Ext.app.Module, {
	id : 'ulog-win',
	init : function() {
		this.launcher = {
			text : 'ULog',
			iconCls : 'icon-grid',
			handler : this.createWindow,
			scope : this
		}
	},
	createWindow : function() {
		var desktop = this.app.getDesktop();
		var win = desktop.getWindow('ulog-win');
		if (!win) {
			win = desktop.createWindow({
				id : 'ulog-win',
				title : 'ULog Window',
				width : 740,
				height : 480,
				iconCls : 'icon-grid',
				shim : false,
				animCollapse : false,
				constrainHeader : true,
				layout : 'fit',
				html : "<div id=\"ulog-container\"></div>"
			});
			MyDesktop.log.logContainer = Ext.get("ulog-container");
			MyDesktop.log.allToContainer();
		}
		win.show();
	}
});

var tobj;
/**
 *log monitor window.
 */
MyDesktop.LogMonitorWindow = Ext.extend(Ext.app.Module, {
	id : 'lmon-win',
	init : function() {
		this.launcher = {
			text : 'Log Monitor',
			iconCls : 'icon-grid',
			handler : this.loadWindow,
			scope : this
		}
	},
	loadWindow : function() {

	},
	createWindow : function(args) {
		if (!args) {
			return;
		}
		var name = args.client + "-" + args.ename;
		console.log(args);
		var desktop = this.app.getDesktop();
		var win = desktop.getWindow(name);
		if (!win) {
			win = desktop.createWindow({
				id : name,
				title : 'ULog Window(' + name + ')',
				width : 740,
				height : 480,
				iconCls : 'icon-grid',
				shim : false,
				animCollapse : false,
				constrainHeader : true,
				layout : 'fit',
				html : "<div style=\"overflow-y:auto;height:100%;margin-left:5\" id=\"lmon-container-" + name + "\"></div>",
				listeners : {
					close : function() {
						win.stopLoad();
					}
				}
			});
			win.wcontainer = Ext.get("lmon-container-" + name);
			win.wname = name;
			win.args = args;
			win.logLength = 0;
			win.loadLog = function() {
				Ext.Ajax.request({
					url : 'EServer/' + win.args.client + '/T_LOG/' + win.args.ename + '/' + win.logLength,
					method : 'GET',
					success : function(response, options) {
						var res = response.responseText;
						var cidx = res.indexOf('\n');
						var cmd = res.substring(0, cidx);
						if (cmd.trim() !== "200") {
							MyDesktop.log.add("[" + win.wname + "] get log failed, msg:" + res.substring(cidx + 1));
							return;
						}
						res = res.substring(cidx + 1);
						cidx = res.indexOf('\n');
						cmd = res.substring(0, cidx);
						res = res.substring(cidx + 1);
						var cmds = cmd.split(' ');
						var lbeg = parseInt(cmds[1]);
						var llen = parseInt(cmds[2]);
						if (lbeg < win.logLength) {
							Ext.each(win.wcontainer.query("div"), function(item, index, len) {
								item.remove();
							});
							win.logLength = lbeg;
						}
						if (llen > 0) {
							tobj=res;
							res=res.replace(/(\r\n|\n|\r)/gm, "<br/>")
							win.logLength += llen;
							win.wcontainer.createChild({
								html : res
							});
						}
					},
					failure : function(response, options) {

					}
				});
				win.startLoad();
			}
			win.startLoad = function() {
				this.timeout = setTimeout(win.loadLog, 10000);
			}
			win.stopLoad = function() {
				clearTimeout(this.timeout);
			}
		}
		win.show();
		win.loadLog();
	}
});
/**
 * log monitor window creater.
 */
MyDesktop.LMonWindow = new MyDesktop.LogMonitorWindow();
/**
 *client manager window.
 */
MyDesktop.ClientManagerWindow = Ext.extend(Ext.app.Module, {
	id : 'cmgr-win',
	init : function() {
		this.launcher = {
			text : 'Client Manager',
			iconCls : 'cmgr',
			handler : this.createWindow,
			scope : this
		}
		this.idx = 1;
	},

	createWindow : function() {
		var desktop = this.app.getDesktop();
		var id = 'cmgr-win' + this.idx;
		var win = desktop.getWindow(id);
		if (!win) {
			var clients, cmds, events;
			var clt_box, cmd_box, ent_box;
			function loadEventData(client, cmd) {
				Ext.Ajax.request({
					url : 'Cmd/List/' + client + '/' + cmd,
					method : 'GET',
					success : function(response, options) {
						var vals = response.responseText.split('\n');
						var rvals = new Array();
						for (var i = 0; i < vals.length; i++) {
							var val = vals[i].trim();
							if (!val) {
								continue;
							}
							rvals[rvals.length] = val;
						}
						var cvals = new Array();
						for (var i = 0; i < rvals.length; i++) {
							cvals.push(new Array(rvals[i]));
						}
						if (cvals.length > 0) {
							events.loadData(cvals);
						}
					},
					failure : function(response, options) {
						Ext.MessageBox.alert('Failed', 'Server error code:' + response.status);
					}
				});
			}

			clients = new Ext.data.ArrayStore({
				fields : ['val'],
				data : []
			});
			clt_box = new Ext.form.ComboBox({
				fieldLabel : 'Online Clients',
				hiddenName : 'Clients',
				store : clients,
				valueField : 'val',
				displayField : 'val',
				typeAhead : true,
				mode : 'local',
				triggerAction : 'all',
				emptyText : 'Select a client...',
				selectOnFocus : true,
				anchor : '98%',
				listeners : {
					select : function(tar, nval) {
						events.loadData([]);
						if (cmd_box && cmd_box.value) {
							loadEventData(nval.data.val, cmd_box.value);
						}
					}
				}
			});
			cmds = new Ext.data.ArrayStore({
				fields : ['val'],
				data : [["T_LOG"], ["N_EVENT"], ["N_SYNC"]]
			});
			cmd_box = new Ext.form.ComboBox({
				fieldLabel : 'Commands',
				hiddenName : 'Commands',
				store : cmds,
				valueField : 'val',
				displayField : 'val',
				typeAhead : true,
				mode : 'local',
				triggerAction : 'all',
				emptyText : 'Select a command...',
				selectOnFocus : true,
				anchor : '98%',
				listeners : {
					select : function(tar, nval) {
						ent_box.clearValue();
						events.loadData([]);
						if (nval.data.val == "N_SYNC") {
							ent_box.disable();
						} else {
							ent_box.enable();
							if (clt_box && clt_box.value) {
								loadEventData(clt_box.value, nval.data.val);
							}
						}
					}
				}
			});
			events = new Ext.data.ArrayStore({
				fields : ['val'],
				data : []
			});
			ent_box = new Ext.form.ComboBox({
				fieldLabel : 'Support Events',
				hiddenName : 'Events',
				store : events,
				valueField : 'val',
				displayField : 'val',
				typeAhead : true,
				mode : 'local',
				triggerAction : 'all',
				emptyText : 'Select a events...',
				selectOnFocus : true,
				anchor : '98%'
			});
			var fs = new Ext.FormPanel({
				baseCls : 'x-plain',
				align : 'center',
				labelAlign : 'right',
				labelWidth : 100,
				bodyStyle : 'margin:15px auto -15px auto',
				items : [new Ext.form.FieldSet({
					title : 'Clients Information',
					autoHeight : true,
					defaultType : 'textfield',
					items : [clt_box, cmd_box, ent_box]
				})],
				buttonAlign : "center",
				buttons : [{
					text : 'Start',
					formBind : false,
					handler : function(btn, evt) {
						if (!cmd_box.value) {
							return;
						}
						switch(cmd_box.value) {
							case "N_SYNC":
								if (!clt_box.value) {
									return;
								}
								Ext.Ajax.request({
									url : 'EServer/' + clt_box.value + '/' + cmd_box.value,
									method : 'GET',
									success : function(response, options) {
										MyDesktop.log.add("sending N_SYNC to client:" + clt_box.value);
									},
									failure : function(response, options) {
										Ext.MessageBox.alert('Failed', 'Server error code:' + response.status);
									}
								});
								break;
							case "N_EVENT":
								if (!clt_box.value || !ent_box.value) {
									return;
								}
								Ext.Ajax.request({
									url : 'EServer/' + clt_box.value + '/' + cmd_box.value + '/' + ent_box.value,
									method : 'GET',
									success : function(response, options) {
										MyDesktop.log.add("sending N_EVENT/" + ent_box.value + " to client:" + clt_box.value);
									},
									failure : function(response, options) {
										Ext.MessageBox.alert('Failed', 'Server error code:' + response.status);
									}
								});
								break;
							case "T_LOG":
								if (!clt_box.value || !ent_box.value) {
									return;
								}
								MyDesktop.LMonWindow.createWindow({
									client : clt_box.value,
									ename : ent_box.value
								});
								break;
						}
					}
				}]
			});
			win = desktop.createWindow({
				id : id,
				title : 'Client Manager Window - ' + this.idx,
				width : 355,
				height : 200,
				border : false,
				iconCls : 'cmgr',
				shim : false,
				animCollapse : false,
				constrainHeader : true,
				plain : true,
				layout : "fit",
				items : fs
			});
			this.idx++;
			var cmgr = Ext.get("cmgr-container");
			Ext.Ajax.request({
				url : 'Cmd/List',
				method : 'GET',
				success : function(response, options) {
					var vals = response.responseText.split('\n');
					var rvals = new Array();
					for (var i = 0; i < vals.length; i++) {
						var val = vals[i].trim();
						if (!val) {
							continue;
						}
						rvals[rvals.length] = val;
					}
					var cvals = new Array();
					for (var i = 0; i < rvals.length; i++) {
						cvals.push(new Array(rvals[i]));
					}
					if (cvals.length > 0) {
						clients.loadData(cvals);
					}
				},
				failure : function(response, options) {
					Ext.MessageBox.alert('Failed', 'Server error code:' + response.status);
				}
			});
		}
		win.show();
	}
});
/**
 * read.
 */
Ext.onReady(function() {
	Ext.Ajax.request({
		url : 'CheckLogin',
		method : 'GET',
		success : function(response, options) {
		},
		failure : function(response, options) {
			MyDesktop.Login();
		}
	});

});

/*
 * Example windows
 */
MyDesktop.GridWindow = Ext.extend(Ext.app.Module, {
	id : 'grid-win',
	init : function() {
		this.launcher = {
			text : 'Grid Window',
			iconCls : 'icon-grid',
			handler : this.createWindow,
			scope : this
		}
	},

	createWindow : function() {
		var desktop = this.app.getDesktop();
		var win = desktop.getWindow('grid-win');
		if (!win) {
			win = desktop.createWindow({
				id : 'grid-win',
				title : 'Grid Window',
				width : 740,
				height : 480,
				iconCls : 'icon-grid',
				shim : false,
				animCollapse : false,
				constrainHeader : true,

				layout : 'fit',
				items : new Ext.grid.GridPanel({
					border : false,
					ds : new Ext.data.Store({
						reader : new Ext.data.ArrayReader({}, [{
							name : 'company'
						}, {
							name : 'price',
							type : 'float'
						}, {
							name : 'change',
							type : 'float'
						}, {
							name : 'pctChange',
							type : 'float'
						}]),
						data : Ext.grid.dummyData
					}),
					cm : new Ext.grid.ColumnModel([new Ext.grid.RowNumberer(), {
						header : "Company",
						width : 120,
						sortable : true,
						dataIndex : 'company'
					}, {
						header : "Price",
						width : 70,
						sortable : true,
						renderer : Ext.util.Format.usMoney,
						dataIndex : 'price'
					}, {
						header : "Change",
						width : 70,
						sortable : true,
						dataIndex : 'change'
					}, {
						header : "% Change",
						width : 70,
						sortable : true,
						dataIndex : 'pctChange'
					}]),

					viewConfig : {
						forceFit : true
					},
					//autoExpandColumn:'company',

					tbar : [{
						text : 'Add Something',
						tooltip : 'Add a new row',
						iconCls : 'add'
					}, '-', {
						text : 'Options',
						tooltip : 'Blah blah blah blaht',
						iconCls : 'option'
					}, '-', {
						text : 'Remove Something',
						tooltip : 'Remove the selected item',
						iconCls : 'remove'
					}]
				})
			});
		}
		win.show();
	}
});

MyDesktop.TabWindow = Ext.extend(Ext.app.Module, {
	id : 'tab-win',
	init : function() {
		this.launcher = {
			text : 'Tab Window',
			iconCls : 'tabs',
			handler : this.createWindow,
			scope : this
		}
	},

	createWindow : function() {
		var desktop = this.app.getDesktop();
		var win = desktop.getWindow('tab-win');
		if (!win) {
			win = desktop.createWindow({
				id : 'tab-win',
				title : 'Tab Window',
				width : 740,
				height : 480,
				iconCls : 'tabs',
				shim : false,
				animCollapse : false,
				border : false,
				constrainHeader : true,

				layout : 'fit',
				items : new Ext.TabPanel({
					activeTab : 0,

					items : [{
						title : 'Tab Text 1',
						header : false,
						html : '<p>Something useful would be in here.</p>',
						border : false
					}, {
						title : 'Tab Text 2',
						header : false,
						html : '<p>Something useful would be in here.</p>',
						border : false
					}, {
						title : 'Tab Text 3',
						header : false,
						html : '<p>Something useful would be in here.</p>',
						border : false
					}, {
						title : 'Tab Text 4',
						header : false,
						html : '<p>Something useful would be in here.</p>',
						border : false
					}]
				})
			});
		}
		win.show();
	}
});

MyDesktop.AccordionWindow = Ext.extend(Ext.app.Module, {
	id : 'acc-win',
	init : function() {
		this.launcher = {
			text : 'Accordion Window',
			iconCls : 'accordion',
			handler : this.createWindow,
			scope : this
		}
	},

	createWindow : function() {
		var desktop = this.app.getDesktop();
		var win = desktop.getWindow('acc-win');
		if (!win) {
			win = desktop.createWindow({
				id : 'acc-win',
				title : 'Accordion Window',
				width : 250,
				height : 400,
				iconCls : 'accordion',
				shim : false,
				animCollapse : false,
				constrainHeader : true,

				tbar : [{
					tooltip : {
						title : 'Rich Tooltips',
						text : 'Let your users know what they can do!'
					},
					iconCls : 'connect'
				}, '-', {
					tooltip : 'Add a new user',
					iconCls : 'user-add'
				}, ' ', {
					tooltip : 'Remove the selected user',
					iconCls : 'user-delete'
				}],

				layout : 'accordion',
				border : false,
				layoutConfig : {
					animate : false
				},

				items : [new Ext.tree.TreePanel({
					id : 'im-tree',
					title : 'Online Users',
					loader : new Ext.tree.TreeLoader(),
					rootVisible : false,
					lines : false,
					autoScroll : true,
					tools : [{
						id : 'refresh',
						on : {
							click : function() {
								var tree = Ext.getCmp('im-tree');
								tree.body.mask('Loading', 'x-mask-loading');
								tree.root.reload();
								tree.root.collapse(true, false);
								setTimeout(function() {// mimic a server call
									tree.body.unmask();
									tree.root.expand(true, true);
								}, 1000);
							}
						}
					}],
					root : new Ext.tree.AsyncTreeNode({
						text : 'Online',
						children : [{
							text : 'Friends',
							expanded : true,
							children : [{
								text : 'Jack',
								iconCls : 'user',
								leaf : true
							}, {
								text : 'Brian',
								iconCls : 'user',
								leaf : true
							}, {
								text : 'Jon',
								iconCls : 'user',
								leaf : true
							}, {
								text : 'Tim',
								iconCls : 'user',
								leaf : true
							}, {
								text : 'Nige',
								iconCls : 'user',
								leaf : true
							}, {
								text : 'Fred',
								iconCls : 'user',
								leaf : true
							}, {
								text : 'Bob',
								iconCls : 'user',
								leaf : true
							}]
						}, {
							text : 'Family',
							expanded : true,
							children : [{
								text : 'Kelly',
								iconCls : 'user-girl',
								leaf : true
							}, {
								text : 'Sara',
								iconCls : 'user-girl',
								leaf : true
							}, {
								text : 'Zack',
								iconCls : 'user-kid',
								leaf : true
							}, {
								text : 'John',
								iconCls : 'user-kid',
								leaf : true
							}]
						}]
					})
				}), {
					title : 'Settings',
					html : '<p>Something useful would be in here.</p>',
					autoScroll : true
				}, {
					title : 'Even More Stuff',
					html : '<p>Something useful would be in here.</p>'
				}, {
					title : 'My Stuff',
					html : '<p>Something useful would be in here.</p>'
				}]
			});
		}
		win.show();
	}
});

// for example purposes
var windowIndex = 0;

MyDesktop.BogusModule = Ext.extend(Ext.app.Module, {
	init : function() {
		this.launcher = {
			text : 'Window ' + (++windowIndex),
			iconCls : 'bogus',
			handler : this.createWindow,
			scope : this,
			windowId : windowIndex
		}
	},

	createWindow : function(src) {
		var desktop = this.app.getDesktop();
		var win = desktop.getWindow('bogus' + src.windowId);
		if (!win) {
			win = desktop.createWindow({
				id : 'bogus' + src.windowId,
				title : src.text,
				width : 640,
				height : 480,
				html : '<p>Something useful would be in here.</p>',
				iconCls : 'bogus',
				shim : false,
				animCollapse : false,
				constrainHeader : true
			});
		}
		win.show();
	}
});

MyDesktop.BogusMenuModule = Ext.extend(MyDesktop.BogusModule, {
	init : function() {
		this.launcher = {
			text : 'Bogus Submenu',
			iconCls : 'bogus',
			handler : function() {
				return false;
			},
			menu : {
				items : [{
					text : 'Bogus Window ' + (++windowIndex),
					iconCls : 'bogus',
					handler : this.createWindow,
					scope : this,
					windowId : windowIndex
				}, {
					text : 'Bogus Window ' + (++windowIndex),
					iconCls : 'bogus',
					handler : this.createWindow,
					scope : this,
					windowId : windowIndex
				}, {
					text : 'Bogus Window ' + (++windowIndex),
					iconCls : 'bogus',
					handler : this.createWindow,
					scope : this,
					windowId : windowIndex
				}, {
					text : 'Bogus Window ' + (++windowIndex),
					iconCls : 'bogus',
					handler : this.createWindow,
					scope : this,
					windowId : windowIndex
				}, {
					text : 'Bogus Window ' + (++windowIndex),
					iconCls : 'bogus',
					handler : this.createWindow,
					scope : this,
					windowId : windowIndex
				}]
			}
		}
	}
});

// Array data for the grid
Ext.grid.dummyData = [['3m Co', 71.72, 0.02, 0.03, '9/1 12:00am'], ['Alcoa Inc', 29.01, 0.42, 1.47, '9/1 12:00am'], ['American Express Company', 52.55, 0.01, 0.02, '9/1 12:00am'], ['American International Group, Inc.', 64.13, 0.31, 0.49, '9/1 12:00am'], ['AT&T Inc.', 31.61, -0.48, -1.54, '9/1 12:00am'], ['Caterpillar Inc.', 67.27, 0.92, 1.39, '9/1 12:00am'], ['Citigroup, Inc.', 49.37, 0.02, 0.04, '9/1 12:00am'], ['Exxon Mobil Corp', 68.1, -0.43, -0.64, '9/1 12:00am'], ['General Electric Company', 34.14, -0.08, -0.23, '9/1 12:00am'], ['General Motors Corporation', 30.27, 1.09, 3.74, '9/1 12:00am'], ['Hewlett-Packard Co.', 36.53, -0.03, -0.08, '9/1 12:00am'], ['Honeywell Intl Inc', 38.77, 0.05, 0.13, '9/1 12:00am'], ['Intel Corporation', 19.88, 0.31, 1.58, '9/1 12:00am'], ['Johnson & Johnson', 64.72, 0.06, 0.09, '9/1 12:00am'], ['Merck & Co., Inc.', 40.96, 0.41, 1.01, '9/1 12:00am'], ['Microsoft Corporation', 25.84, 0.14, 0.54, '9/1 12:00am'], ['The Coca-Cola Company', 45.07, 0.26, 0.58, '9/1 12:00am'], ['The Procter & Gamble Company', 61.91, 0.01, 0.02, '9/1 12:00am'], ['Wal-Mart Stores, Inc.', 45.45, 0.73, 1.63, '9/1 12:00am'], ['Walt Disney Company (The) (Holding Company)', 29.89, 0.24, 0.81, '9/1 12:00am']];
