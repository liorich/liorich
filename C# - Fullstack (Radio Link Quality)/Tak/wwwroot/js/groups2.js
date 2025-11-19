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
        this.plats.splice(this.plats.indexOf(plat_id),1);
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
        var master = "<b>Master</b>"; if (!object.host_report.master) master = "Slave"
        var onePPS = "1PPS Valid"; if(!object.host_report.onePPSValid) onePPS= '<b><p style="color:#FF0000; display:inline">1PPS Invalid</p></b>'
        // Update Host Status
        document.getElementById("Host_" + this.id).children["HostLine"].children["Host"].children["Host_info"].innerHTML =
            master + " | " + onePPS + "<br />UP: F" + object.host_report.upFreq + " , DOWN: F" + object.host_report.downFreq + "<br />"
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
            // Color bad squares?

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
            if (adt_data.a1rssi - adt_data.a2rssi < -5) content.children[1].children[0].style.backgroundColor = content.children[1].children[1].style.backgroundColor = "orange";
            if (adt_data.a2rssi - adt_data.a1rssi < -5) content.children[2].children[0].style.backgroundColor = content.children[2].children[1].style.backgroundColor = "orange";
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
            if(elm != null) elm.remove();
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

        for (var r = 0; r < numNodes+1; r++) {
            var row = table.insertRow(-1);
            for (var c = 0; c < numNodes+1; c++) {
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
                table.children[0].children[r].children[c].style.backgroundColor = this.getDomoColor(qual[r -1][c - 1]);
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
            addMsg("Platform " + item + " has timed out or went offline.",1);
            Groups_vars.forEach((group) => {
                group.removePlatform(item);
            });
        }
        if (object[item] == "host") {
            addMsg("Group host " + item + " has timed out or went offline.",0);
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