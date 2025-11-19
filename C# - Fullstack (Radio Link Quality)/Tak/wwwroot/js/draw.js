class Report {
    constructor(host_id, plat_id, adt_a1snr, adt_a2snr, adt_a1rs, adt_a2rs, gdt_a1snr, gdt_a2snr, gdt_a1rs, gdt_a2rs, com, time) {
        // Holds a basic level data point in the browser
        this.time = time;
        this.host_id = host_id;
        this.plat_id = plat_id;
        this.adt_a1snr = adt_a1snr;
        this.adt_a2snr = adt_a2snr;
        this.adt_a1rs = adt_a1rs;
        this.adt_a2rs = adt_a2rs;
        this.gdt_a1snr = gdt_a1snr;
        this.gdt_a2snr = gdt_a2snr;
        this.gdt_a1rs = gdt_a1rs;
        this.gdt_a2rs = gdt_a2rs;
        this.com = com;
    }
}

class GraphTable {
    constructor(table_id, plat_id, host_id, main=true) {
        this.table_id = table_id;
        this.plat_id = plat_id;
        this.host_id = host_id;
        this.type = "RSSI";
        this.unit = " [dBm]";
        this.reportArray = []; // 2D array;
        this.isLarge = false;
        var template = document.getElementById("tmp_GraphBox").content.children["Graph_"].cloneNode(true);
        template.id = "Graph_" + table_id;
        template.children[0].children[1].addEventListener('mousedown', (e) => {
            this.toggleSNR();
            this.initialize();
        });
        template.children[0].children[2].addEventListener('mousedown', (e) => {
            this.toggleRSSI();
            this.initialize();
        });
        template.children[0].children[3].addEventListener('mousedown', (e) => {
            template.remove();
            var idx = tables_ids.indexOf(this.table_id);
            tables_ids.splice(idx, 1);
            tables.splice(idx, 1);
            Groups_vars.forEach((group) => { group.refreshLines() })
        });
        if (main == true) document.getElementById("DB2_MainContainer").appendChild(template);
        else document.getElementById("LogRequestDisplay").appendChild(template);
        Groups_vars.forEach((group) => { group.refreshLines() })
        this.initialize();
    }
    toggleSNR() {
        this.type = "SNR";
        this.unit = " [dB]";
    }
    toggleRSSI() {
        this.type = "RSSI";
        this.unit = " [dBm]";
    }
    initialize() {
        document.getElementById("Graph_" + this.table_id).children[0].children[0].innerHTML = "GDT-" + this.host_id + " & A-" + this.plat_id
            + " Ant1/2 " + this.type + this.unit;
        // Initialize Params
        var variables = ["a1", "a2", "g1", "g2"];
        var titles = ["ADT A1" + this.type, "ADT A2" + this.type, "GDT A1" + this.type, "GDT A2" + this.type];
        var colors = ["#325aa8", "#698bd1", "#c9b020", "#8a7811"];
        var legend_container = document.getElementById("Graph_" + this.table_id).children[1];
        legend_container.innerHTML = "";
        for (let i = 0; i < 4; i++) {
            var template = document.getElementById("tmp_GraphLegend").content.children["Graph_Legend_"].cloneNode(true);
            template.id = "Graph_Legend_" + variables[i];
            template.children[0].style.backgroundColor = colors[i];
            template.children[1].innerHTML = titles[i];
            legend_container.appendChild(template);
        }

        // Initialize Values and Graph
        var constants = [];
        if (this.type == "RSSI") constants = [-0, -25, -50, -75, -100];
        else constants = [32, 24, 16, 8, 0];
        var min = Math.min(...constants);
        var max = Math.max(...constants);
        var range = Math.abs(max - min);
        var markers = document.getElementById("Graph_" + this.table_id).children[2].children[0]; // Value Markers
        markers.width = 40;
        markers.height = 450;
        var m_width = markers.width;
        var m_height = markers.height;
        var ctx = markers.getContext("2d");
        ctx.clearRect(0, 0, m_width, m_height);
        ctx.font = "bold 15pt Arial";
        for (let i = 0; i < 5; i++) {
            if (this.type == "RSSI") ctx.fillText(constants[i], 0, (Math.abs(constants[i]) / range) * m_height);
            else ctx.fillText(constants[i], 0, ((32 - Math.abs(constants[i])) / range) * m_height);
        }
        ctx.restore();
        this.draw();
    }
    draw() {
        // Live feature: so that the array doesn't explode, delete first 60 points when data > 900 (15 mins)
        if (this.reportArray.length > 900) {
            this.reportArray.splice(0, 60);
        }
        // Draw gray lines
        var constants = [];
        if (this.type == "RSSI") constants = [-0, -25, -50, -75, -100];
        else constants = [32, 24, 16, 8, 0];
        var content = document.getElementById("Graph_" + this.table_id).children[2].children[1]; // Display
        var c_width = content.width;
        var c_height = content.height;
        var min = Math.min(...constants);
        var max = Math.max(...constants);
        var range = Math.abs(max - min);
        content.getContext("2d").clearRect(0, 0, c_width, c_height);

        for (let i = 0; i < 5; i++) {
            var ctx = content.getContext("2d");
            ctx.beginPath(); ctx.strokeStyle = "dimgray";
            ctx.moveTo(0, (Math.abs(constants[i]) / range) * c_height);
            ctx.lineTo(c_width, (Math.abs(constants[i]) / range) * c_height);
            ctx.stroke();
        }
        // Draw all data points
        var colors = ["#325aa8", "#698bd1", "#c9b020", "#8a7811"];
        var var_names = [];
        if (this.type == "RSSI") var_names = ["adt_a1rs", "adt_a2rs", "gdt_a1rs", "gdt_a2rs"];
        else var_names = ["adt_a1snr", "adt,a2snr", "gdt_a1snr", "gdt_a2snr"];
        var num_points = this.reportArray.length; 
        this.reportArray.forEach((report, idx) => {
            if (idx == 0) {
                var a = 1;
            }
            else {
                var prev_report = this.reportArray[idx - 1];
                for (let j = 0; j < 4; j++) {
                    var item = report[var_names[j]];
                    var prev = prev_report[var_names[j]]
                    var ctx = content.getContext("2d");
                    ctx.beginPath();
                    ctx.strokeStyle = colors[j];
                    if (this.type == "RSSI") {
                        ctx.moveTo(((idx - 1) / num_points) * c_width, (Math.abs(prev) / range) * c_height);
                        ctx.lineTo((idx / num_points) * c_width, (Math.abs(item) / range) * c_height);
                    }
                    else {
                        ctx.moveTo(((idx - 1) / num_points) * c_width, ((32-Math.abs(prev)) / range) * c_height);
                        ctx.lineTo((idx / num_points) * c_width, ((32-Math.abs(item)) / range) * c_height);
                    }
                    ctx.filter = 'none';
                    ctx.stroke();
                    
                }
                // Comment exists?
                if (report["com"] != "") {
                    var ctx = content.getContext("2d");
                    ctx.beginPath();
                    ctx.strokeStyle = "red";
                    ctx.moveTo((idx / num_points) * c_width, c_height);
                    ctx.lineTo((idx / num_points) * c_width, 1/10 * c_height);
                    ctx.filter = 'none';
                    ctx.stroke();

                    var ctx = content.getContext("2d");
                    ctx.font = "10px Arial";
                    ctx.fillText(report["com"], (idx / num_points) * c_width, 1 / 10 * c_height);
                }
                
            }
        });

        var timestamps = document.getElementById("Graph_" + this.table_id).children[3].children[1];
        timestamps.width = 650;
        timestamps.height = 80;
        var t_width = timestamps.width;
        var t_height = timestamps.height;
        timestamps.getContext("2d").clearRect(0, 0, t_width, t_height);
        for (let k = 0; k < this.reportArray.length; k += 1 + Math.round(this.reportArray.length / 6)) {
            // Timestamp it
             var report = this.reportArray[k];
             var ctx = timestamps.getContext("2d");
             ctx.font = "14pt Arial";
            ctx.fillText("|" + report["time"], (k / num_points) * t_width, t_height / 2);
            var ctx2 = content.getContext("2d");
            ctx2.strokeStyle = "lightgray";
            ctx2.lineWidth = 1;
            ctx2.moveTo((k / num_points) * c_width, c_height);
            ctx2.lineTo((k / num_points) * c_width, 1 / 10 * c_height);
            ctx2.filter = 'none';
            ctx2.stroke();
        }
        

    }
}

function dashboardTrackPlatform() {
    // If prompt is already open, return
    if (document.getElementById("DB2_LogFunc_TrackWindow")) return; 
    // Add the prompt
    var template = document.getElementById("tmp_TrackPrompt").content.children["DB2_LogFunc_TrackWindow"].cloneNode(true);
    var adts = [];
    Groups_vars.forEach((host) => {
        host.plats.forEach((plat) => { adts.push(plat); })
    });
    if (adts.length == 0) {
        addMsg("There are no available online ADTs to track.",1);
        return;
    }
    var options = template.children[1].children[1];
    adts.forEach((adt) => {
        var option = document.createElement("option");
        option.text = adt;
        options.add(option);
    });
    template.children[0].children[1].addEventListener("mousedown", (e) => template.remove());

    var container = document.getElementById("DB2_LogFuncContainer");
    container.appendChild(template);
}

let tables = [];
let tables_ids = [];
function dashboardAddRequest() {
    var adt = document.getElementById("DB2_LogFunc_TrackWindow_Select").value;
    if (tables_ids.includes(adt)) {
        addMsg("There is already an active tracker for this ADT.", 1);
        return;
    }
    tables.push(new GraphTable(adt, 6, adt));
    tables_ids.push(adt);
    document.getElementById("DB2_LogFunc_TrackWindow").remove();
}

function openCommentWindow() {
    if (document.getElementById("DB2_LogFunc_AddComment")) return;
    // Add the prompt
    var template = document.getElementById("tmp_AddComment").content.children["DB2_LogFunc_AddComment"].cloneNode(true);
    var adts = [];
    Groups_vars.forEach((host) => {
        host.plats.forEach((plat) => { adts.push(plat); })
    });
    if (adts.length == 0) {
        addMsg("There are no available online ADTs to comment on.", 1);
        return;
    }
    var options = template.children[1].children[0].children[1];
    adts.forEach((adt) => {
        var option = document.createElement("option");
        option.text = adt;
        options.add(option);
    });
    template.children[0].children[1].addEventListener("mousedown", (e) => template.remove());
    var container = document.getElementById("DB2_LogFuncContainer");
    container.appendChild(template);
}
function CommentProceed() {
    var elm = document.getElementById("DB2_LogFunc_CommentBox");
    var adt = document.getElementById("DB2_LogFunc_AddComment_Select").value;
    document.getElementById("DB2_LogFunc_AddComment_Content").children[0].style.visibility = "hidden";
    document.getElementById("DB2_LogFunc_AddComment_Content").children[1].style.visibility = "visible";
    elm.placeholder = "Comment on A-" + adt;
    elm.addEventListener('keyup', (event) => {
        if (event.key === 'Enter') socketSendComment();
    });
}

function chAntWindow() {
    
    if (document.getElementById("DB2_LogFunc_chAnt_prompt")) return;
    // Add the prompt
    var template = document.getElementById("tmp_chAnt").content.children["DB2_LogFunc_chAnt_prompt"].cloneNode(true);
    if (Groups_ids.length == 0) {
        addMsg("There are no available online GDTs to request Tx Ant change.", 1);
        return;
    }
    var options = template.children[1].children[1];
    Groups_ids.forEach((gdt) => {
        var option = document.createElement("option");
        option.text = gdt;
        options.add(option);
    });
    template.children[0].children[1].addEventListener("mousedown", (e) => template.remove());
    var container = document.getElementById("DB2_LogFuncContainer");
    container.appendChild(template);
}