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
    constructor(table_id, plat_id, host_id, main = true) {
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
                        ctx.moveTo(((idx - 1) / num_points) * c_width, ((32 - Math.abs(prev)) / range) * c_height);
                        ctx.lineTo((idx / num_points) * c_width, ((32 - Math.abs(item)) / range) * c_height);
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
                    ctx.lineTo((idx / num_points) * c_width, 1 / 10 * c_height);
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
        addMsg("There are no available online ADTs to track.", 1);
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

class GroupContainer {
    constructor(host_id, host_name) {
        this.id = host_id;
        this.name = host_name
        this.plats = [];
        this.knownPlats = 0;
        // Create platforms element
        var template = document.getElementById("tmp_Container").content.children["Host_Main_"].cloneNode(true);
        template.id = "Host_Main_" + host_id;
        template.children["Host_"].id = "Host_" + host_id;
        if (host_name == "") template.children["Host_" + host_id].children["HostLine"].children["Host"].children["Host_name"].innerHTML = "GDT-" + host_id;
        else template.children["Host_" + host_id].children["HostLine"].children["Host"].children["Host_name"].innerHTML = host_name + host_id;

        // Add to the view
        document.getElementById("DB2_MainContainer").appendChild(template);

        // Domo Extension
        this.hasDomo = -1;
        this.DomoElement = null;

    }
    addPlatform(plat_id, plat_name) {
        this.removeLines();
        this.plats.push(plat_id);
        // Create Platform Element
        var template = document.getElementById("tmp_Platform").content.children["Plat_"].cloneNode(true);
        template.id = "Plat_" + plat_id;
        if (plat_name == "") template.children["Plat_name"].innerHTML = "A-" + plat_id;
        else template.children["Plat_name"].innerHTML = plat_name + "|" + plat_id;


        // Create Link Element
        var qual_template = document.getElementById("tmp_Link").content.children["Link_"].cloneNode(true);
        qual_template.id = "Link_" + plat_id;
        qual_template.children["Data"].children[0].children[0].children[0].innerHTML = "A-" + plat_id;

        // Add highlight Event
        template.addEventListener("mouseover", (e) => {
            template.style.borderColor = "Cyan";
            qual_template.style.borderColor = "Cyan";
        });
        qual_template.addEventListener("mouseover", (e) => {
            template.style.borderColor = "Cyan";
            qual_template.style.borderColor = "Cyan";
        });
        template.addEventListener("mouseout", (e) => {
            template.style.borderColor = "dimgray";
            qual_template.style.borderColor = "dimgray";
        });
        qual_template.addEventListener("mouseout", (e) => {
            template.style.borderColor = "dimgray";
            qual_template.style.borderColor = "dimgray";
        });

        document.getElementById("Host_" + this.id).children["Plats"].appendChild(template);
        document.getElementById("Host_Main_" + this.id).children["Host_LinkQuals"].appendChild(qual_template);

        // Create Quality Line
        this.drawLines();

        this.knownPlats++;
        return;
    }
    removePlatform(plat_id) {
        if (!this.plats.includes(plat_id)) return;
        // Remove all 3
        this.removeLines();
        this.plats.splice(this.plats.indexOf(plat_id), 1);
        document.getElementById("Plat_" + plat_id).remove();
        document.getElementById("Link_" + plat_id).remove();
        this.drawLines();
        this.knownPlats--;

        // Domo
        if (this.hasDomo == plat_id) this.removeDomo();
        return;
    }
    Refresh() {
        var plats = this.plats;
        plats.forEach((plat, idx) => {
            this.removePlatform(plat);
        });
    }
    Update(object) {
        // Update Host Status
        document.getElementById("Host_" + this.id).children["HostLine"].children["Host"].children["Host_info"].innerHTML =
            "UP: F" + object.host_report.upFreq + " , DOWN: F" + object.host_report.downFreq + "<br />"
            + "TX Antenna: ANT" + object.host_report.txAnt + " , Network ID: " + object.host_report.netID;
        var overallLQ = 0;
        if ((Object.keys(object.host_assigned_plats).length) < this.knownPlats || (Object.keys(object.link_receiving_plats).length) < this.knownPlats) {
            this.Refresh();
            return;
        }
        for (var platKey in object.host_assigned_plats) {
            // Plat exists?
            if (!this.plats.includes(platKey)) {
                this.addPlatform(platKey, object.host_assigned_plats[platKey].plat_name);
            }

            // Update ADT Rx
            var adt_data = object.host_assigned_plats[platKey].link_receiving_host;
            var adt = document.getElementById("Plat_" + platKey);
            var content = adt.children[1].children[0].children[0];
            //console.log(content.children[1].children[1]);
            content.children[1].children[1].innerHTML = adt_data.a1rssi;
            content.children[1].children[2].innerHTML = adt_data.a1snr;
            content.children[2].children[1].innerHTML = adt_data.a2rssi;
            content.children[2].children[2].innerHTML = adt_data.a2snr;
            adt.children["Plat_ms"].innerHTML = adt_data.ping_rtt + "ms";
            adt.children[3].style.color = "red";
            if (adt_data.ping_rtt == "-1") adt.children["Plat_ms"].innerHTML = "Unreachable";
            if (adt_data.link_uncontinuous == true && adt_data.ping_rtt != "-1") adt.children[3].innerHTML = "Link Uncontinuous (<80%)";
            else if (adt_data.link_busy == true) adt.children[3].innerHTML = "Link Busy (RTT > 500ms)";
            else {
                adt.children[3].innerHTML = "...";
                adt.children[3].style.color = "black";
            }
            content.style.backgroundColor = this.getLQColorDim(adt_data.LinkQuality);
            // Update GDT Rx
            var gdt_data = object.link_receiving_plats[platKey];
            var gdt_link = document.getElementById("Link_" + platKey);
            content = gdt_link.children[0].children[0];
            content.children[1].children[1].innerHTML = gdt_data.a1rssi;
            content.children[1].children[2].innerHTML = gdt_data.a1snr;
            content.children[2].children[1].innerHTML = gdt_data.a2rssi;
            content.children[2].children[2].innerHTML = gdt_data.a2snr;
            content.style.backgroundColor = this.getLQColorDim(gdt_data.LinkQuality);
            //var link = getElementById("Link_" + platKey);
            overallLQ += (gdt_data.LinkQuality + adt_data.LinkQuality) / 2;

            // Domo extension
            if (object.host_assigned_plats[platKey].plat_mesh != null) {
                if (object.host_assigned_plats[platKey].plat_mesh.shortReport != null) {
                    var rep = object.host_assigned_plats[platKey].plat_mesh.shortReport;
                    if (this.hasDomo == -1) this.addDomo(platKey);
                    this.updateDomo(rep);
                    document.getElementById("Plat_" + platKey).children[0].style.color = "dodgerblue";
                }
            }
            else document.getElementById("Plat_" + platKey).children[0].style.color = "black";

            // Finally, if adt is being tracked, feed data
            if (tables_ids.includes(platKey)) {
                var table = tables[tables_ids.indexOf(platKey)];
                table.reportArray.push(new Report(this.id, platKey, adt_data.a1snr, adt_data.a2snr, adt_data.a1rssi, adt_data.a2rssi, gdt_data.a1snr, gdt_data.a2snr, gdt_data.a1rssi, gdt_data.a2rssi, "", getTime()));
                table.draw();
            }


        }
        overallLQ = Math.ceil(overallLQ / this.plats.length);
        document.getElementById("Host_" + this.id).children["HostLine"].children["Host"].children["Host_name"].style.color = this.getLQColorDim(overallLQ);


        return;
    }
    removeSelf() {
        document.getElementById("Host_Main_" + this.id).remove();
    }
    getLine(idx, div1, div2, color, thickness) {
        var off1 = this.getOffset(div1);
        var off2 = this.getOffset(div2);
        var x1 = off1.left + off1.width / 2;
        var y1 = off1.top + off1.height;
        var x2 = off2.left + off2.width / 2;
        var y2 = off2.top;

        var length = Math.sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

        var cx = ((x1 + x2) / 2) - (length / 2);
        var cy = ((y1 + y2) / 2) - (thickness / 2);

        var angle = Math.atan2((y1 - y2), (x1 - x2)) * (180 / Math.PI);

        cx = cx - 180;

        var htmlLine = "<div class='line' id='line" + idx + "' style='padding:0px; margin:0px; height:" + thickness + "px; background-color:" + color + "; line-height:1px; position:absolute; left:" + cx + "px; top:" + cy + "px; width:" + length + "px; -moz-transform:rotate(" + angle + "deg); -webkit-transform:rotate(" + angle + "deg); transform:rotate(" + angle + "deg);' />";

        return htmlLine;
    }
    getOffset(el) {
        var rect = el.getBoundingClientRect();
        return {
            left: rect.left + window.pageXOffset,
            top: rect.top + window.pageYOffset,
            width: rect.width || el.offsetWidth,
            height: rect.height || el.offsetHeight
        };
    }
    removeLines() {
        this.plats.forEach((id) => {
            var elm = document.getElementById("line" + id);
            if (elm != null) elm.remove();
        });
    }
    drawLines() {
        this.plats.forEach((id) => {
            var host_div = document.getElementById("Host_" + this.id).children["HostLine"].children["Host"];
            var plat_div = document.getElementById("Plat_" + id);
            var line = this.getLine(id, host_div, plat_div, "black", 3);
            var line_template = document.createElement("template");
            line_template.innerHTML = line;
            document.getElementById("Host_" + this.id).appendChild(line_template.content.children[0]);
        });
    }
    refreshLines() {
        this.removeLines();
        this.drawLines();
    }
    getLQColorDim(LQ) {
        var color = "dimgray";
        switch (LQ) {
            case 3: color = "#578a64"; break;
            case 2: color = "#8a8a57"; break;
            case 1: color = "#8a7257"; break;
            case 0: color = "#8a5757"; break;
        }
        return color;
    }
    addDomo(plat_id) {
        this.hasDomo = plat_id;
        // Domo - Add Element
        var template = document.getElementById("tmp_MeshBox").content.children["Mesh_"].cloneNode(true);
        template.id = "Mesh_" + plat_id;
        // Save pointer
        this.DomoElement = template;
        // Add to the view
        document.getElementById("Host_" + this.id).children["Plats"].appendChild(template);

    }
    updateDomo(obj_data) {
        // Exists, then update it
        var data = obj_data;
        this.DomoElement.children[0].children[0].innerHTML = "Mesh " + data.MeshID;
        var description = "";
        for (var item in data.Nodes) description += item + " " + data.Nodes[item] + "<br>";
        this.DomoElement.children[0].children[1].innerHTML = description;
        var info = data[DomoView];
        var qual = data.Qual;
        var numNodes = data.NumNodes;

        // Domo - Add Table
        var table = document.createElement("table");
        table.className = "DomoMat";

        for (var r = 0; r < numNodes + 1; r++) {
            var row = table.insertRow(-1);
            for (var c = 0; c < numNodes + 1; c++) {
                var cell = row.insertCell(-1);
                if (r == c && r > 0) table.children[0].children[r].children[c].style.backgroundColor = "gray";
            }
        }
        table.children[0].children[0].children[0].innerHTML = DomoView;

        var ids = [];
        for (var x in data.Nodes) ids.push(x);
        for (var x = 1; x < numNodes + 1; x++) {
            table.children[0].children[x].children[0].innerHTML = table.children[0].children[0].children[x].innerHTML = ids[x - 1];
            table.children[0].children[x].children[0].style.backgroundColor = table.children[0].children[0].children[x].style.backgroundColor = "lightgray";
        }

        for (var r = 1; r < numNodes + 1; r++) {
            for (var c = 1; c < numNodes + 1; c++) {
                table.children[0].children[r].children[c].innerHTML = info[r - 1][c - 1];
                table.children[0].children[r].children[c].style.backgroundColor = this.getDomoColor(qual[r - 1][c - 1]);
            }
        }
        this.DomoElement.children[1].innerHTML = table.outerHTML;
        this.refreshLines();
    }
    removeDomo() {
        if (this.hasDomo == -1) return;
        this.DomoElement.remove();
        this.hasDomo = -1;
        this.refreshLines();
    }
    getDomoColor(qual) {
        var color = "dimgray";
        switch (qual) {
            case 6: color = "#34ebe8"; break;
            case 5: color = "#34eb74"; break;
            case 4: color = "#abeb34"; break;
            case 3: color = "#ebe834"; break;
            case 2: color = "#eb9834"; break;
            case 1: color = "#eb3434"; break;
            case 0: color = "white"; break;
        }
        return color;
    }
}

// Main
let Groups_ids = [];
let Groups_vars = [];
let DomoIdx = 0;
let DomoView = "SNR";
let DomoViews = ["SNR", "LvA", "LvB"];

setInterval(rotateDomoView, 5000);

function rotateDomoView() {
    DomoIdx = (DomoIdx + 1) % 3;
    DomoView = DomoViews[DomoIdx];
}

function processUpdates(object) {
    // if group doesn't exist:
    if (!Groups_ids.includes(object.host_id)) {
        Groups_ids.push(object.host_id);
        Groups_vars.push(new GroupContainer(object.host_id, object.host_name));
        Groups_vars.forEach((group) => { group.refreshLines() })
    }
    var Group = Groups_vars[Groups_ids.indexOf(object.host_id)];
    Group.Update(object);

}

function handleTimeouts(object) {
    for (item in object) {
        if (object[item] == "plat") {
            addMsg("Platform " + item + " has timed out or went offline.", 1);
            Groups_vars.forEach((group) => {
                group.removePlatform(item);
            });
        }
        if (object[item] == "host") {
            addMsg("Group host " + item + " has timed out or went offline.", 0);
            var idx = Groups_ids.indexOf(parseInt(item));
            var group = Groups_vars[idx];
            group.removeSelf();
            Groups_ids.splice(idx, 1);
            Groups_vars.splice(idx, 1);
        }
    }
}
function addMsg(msg, urgency) {
    var color;
    switch (urgency) {
        case 0: color = "red"; break;
        case 1: color = "orange"; break;
        case 2: color = "yellow"; break;
        case 3: color = "green"; break;
    }
    var container = document.getElementById("DB2_msgs_container");
    container.innerHTML = '<div class="Server_MSG" style="color: ' + color + '; font-style: italic; text-align: left;">' + getTime() + " | " + msg + '</div>' + container.innerHTML;
}

// Draw lines on every window-size change
window.addEventListener('resize', function (event) {
    Groups_vars.forEach((group) => {
        group.removeLines();
        group.drawLines();
    });
});

const socket = new WebSocket("wss://localhost:7256/ws");
// Connection opened
socket.onopen = function (event) {
    addMsg("Websocket connection established", 3);
}

socket.onmessage = function (event) {
    var ob = JSON.parse(event["data"]);
    var type_ob = ob["type"];
    var ob_data = ob[type_ob];

    switch (type_ob) {
        case "Data":
            for (group in ob_data.groups) {
                processUpdates(ob_data.groups[group]);
            }
            break;
        case "MeshReport":
            Groups_vars.forEach((group) => { group.updateDomo(ob_data); });
            break;
        case "Message":
            addMsg(ob_data);
            break;
        case "Timeouts":
            handleTimeouts(ob_data);
            break;

    }
}

socket.onerror = function (event) {
    addMsg("Failed to form websocket connection", 1);
}

socket.onclose = function (event) {
    console.log("Socket was closed");
    addMsg("Websocket connection was ended", 1);
}

function timeStringToSeconds(timeStr) {
    var a = timeStr.split(':');
    var seconds = (+a[0]) * 3600 + (+a[1]) * 60 + (+a[2]);
    return seconds;
}