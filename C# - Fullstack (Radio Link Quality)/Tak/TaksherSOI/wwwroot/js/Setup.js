
class NetworkElm {
    constructor(boxElement, rootElement) {
        this.boxElement = boxElement;
        this.rootElement = rootElement;
        this.Init();
    }
    Init() {
        // Starts root
        this.boxElement.style.justifyContent = "start";
        this.boxElement.children[0].addEventListener('mousedown', (e) => {
            NetworkElements.splice(NetworkElements.indexOf(this.boxElement), 1);
            this.boxElement.remove();
        });
        tmp = this.boxElement;
        console.log(this.boxElement);
        this.rootElement.Init();
    }
}
class NodeElm {
    constructor(elm, deep) {
        this.elm = elm;
        this.node = null;
        this.remotes = [];
        this.deep = deep;
        this.poll = document.getElementById("tmp_SetupPoll").content.children["DB2_SetupPoll"].cloneNode(true);
        document.getElementById("DB2_SetupDisplay").appendChild(this.poll);
        if (this.deep == 0) {
            this.poll.children[3].children[1].value = "base";
            this.poll.children[3].children[1].disabled = true;
        }
        else {
            this.poll.children[3].children[1].value = "remote";
            this.poll.children[3].children[1].disabled = true;
        }
        this.poll.children[12].children[0].addEventListener('mousedown', (e) => {
            var node = checkNodeValidity(this.poll);
            if (node != null) {
                this.node = node;
                this.poll.style.visibility = "hidden";
                this.elm.children[0].children[0].innerHTML = this.node.Name;
                this.elm.children[0].children[0].style.color = "dimgray";
                this.elm.children[1].style.opacity = "1";
                this.elm.children[1].addEventListener('mousedown', (e) => {
                    this.Next();
                });
            }
        });
        this.elm.children[0].addEventListener('mousedown', (e) => {
            this.Show();
        });
        console.log(this.elm.children[0]);
        console.log(this.elm.children[1]);
        
        this.poll.children[12].children[2].addEventListener('mousedown', (e) => {
            this.poll.style.visibility = "hidden";
        });
        this.Init();
    }
    Init() {
        this.elm.style.border = "solid dimgray 3px";
        //this.poll.children[3].children[1].value = "base";
        this.poll.style.visibility = "hidden";
        //this.Show();
    }
    Show() {
        if (this.poll.style.visibility = "visible") return;
        /*
        if (this.node != null) {
            this.poll.children[1].children[1].value = this.node.Name;
            this.poll.children[5].children[1].value = this.node.ShfID;
            this.poll.children[6].children[1].value = this.node.MeshIP;
            this.poll.children[7].children[1].value = this.node.MeshUsername;
            this.poll.children[8].children[1].value = this.node.MeshPW;
            this.poll.children[9].children[1].value = this.node.LinkCheckIP;
            this.poll.children[10].children[1].value = this.node.HasExtendedMesh;
            this.poll.children[3].children[1].value = this.node.StationType;
            this.poll.children[4].children[1].value = this.node.ConnectionType;
            this.poll.children[2].children[1].value = this.node.GraphType;
        }
        */
        this.poll.style.visibility = "visible";
    }
    Next() { 
        var boxElement = this.elm.parentNode.parentNode;
        var newElm = document.getElementById("tmp_SetupContainer").content.children[0].cloneNode(true);
        boxElement.appendChild(newElm);
        var template2 = document.getElementById("tmp_SetupNode").content.children["SetupNode"].cloneNode(true);
        var container = boxElement.children[1 + this.deep + 1];
        container.appendChild(template2);
        this.remotes.push(new NodeElm(template2, this.deep + 1));
    }
}

class AnyNode {
    constructor(Name, ShfID, MeshIP, MeshUsername, MeshPW, LinkCheckIP, HasExtendedMesh, StationType, ConnectionType, GraphType) {

        // Holds a basic level data point in the browser
        this.Name = Name;
        this.ShfID = ShfID;
        this.MeshIP = MeshIP
        this.MeshUsername = MeshUsername;
        this.meshPW = MeshPW;
        this.LinkCheckIP = LinkCheckIP;
        this.HasExtendedMesh = HasExtendedMesh;
        this.StationType = StationType;
        this.ConnectionType = ConnectionType;
        this.GraphType = GraphType;
        this.RemotesNames = []
    }
}

let NetworkElements = [];
let tmp = null;

function SetupAddNetwork() {
    var template = document.getElementById("tmp_DB2_SetupCollection").content.children["DB2_SetupCollectionDiv"].cloneNode(true);
    var template2 = document.getElementById("tmp_SetupNode").content.children["SetupNode"].cloneNode(true);
    template.innerHTML += '<button class="DB2_SetupCollectionRemoveBtn">Remove</button>';
    template.innerHTML = template.innerHTML + '<div class="DB2_Setup_Container"></div>';
    template.children[1].appendChild(template2);
    var boxElement = template;
    var rootElement = template2;
    document.getElementById("DB2_SetupDisplay").prepend(template);
    NetworkElements.push(new NetworkElm(boxElement, new NodeElm(rootElement, 0)));
}

function InitNodeSetup() {
    NetworkElements = [];
    document.getElementById("DB2_SetupDisplay").innerHTML = '<div class="DB2_SetupCollection" onclick="SetupAddNetwork()">Add Network</div>';
}
function SaveSetup() {
    var nodes = [];
    NetworkElements.forEach((item, idx) => {
        var root = item.rootElement;
        setRemoteNames(root);
        nodes.push(root.node);
        var other = getNodeList(root);
        if (other.length > 0) {
            other.forEach((extra) => {
                nodes.push(extra);
            });
        }
    });
    var dict = new Map();
    nodes.forEach((node, idx) => {
        dict.set(node.Name, node)
    });
    socketSendSetup(dict);
}

function setRemoteNames(elm) {
    var arr = []
    elm.remotes.forEach((rem) => {
        arr.push(rem.node.Name);
        setRemoteNames(rem);
    });
    elm.node.RemotesNames = arr;
}
function getNodeList(root) {
    var nodes = [];
    root.remotes.forEach((rem) => {
        nodes.push(rem.node);
        var other = getNodeList(rem);
        if (other.length > 0) {
            other.forEach((extra) => {
                nodes.push(extra);
            });
        }
    });
    return nodes;
}

function checkNodeValidity(elm) {
    var err = elm.querySelector("#SetupPollError");
    if (elm.querySelector("#name").value == "") {
        err.innerHTML = "Node must have a name.";
        return null;
    }
    if (elm.querySelector("#stationType").value == "none") {
        err.innerHTML = "Define Base or Remote.";
        return null;
    }
    if (elm.querySelector("#graphType").value == "none") {
        err.innerHTML = "Define Graph type.";
        return null;
    }
    if (elm.querySelector("#connectionType").value == "none") {
        err.innerHTML = "Define Connection Type.";
        return null;
    }
    if (elm.querySelector("#connectionType").value == "shf" && elm.querySelector("#ShfID").value == "") {
        err.innerHTML = "Shahaf Node's Platform ID must be defined.";
        return null;
    }
    if (elm.querySelector("#connectionType").value == "dtc" && elm.querySelector("#MeshIP").value == "") {
        err.innerHTML = "Provide the DTC Node's IP.";
        return null;
    }
    if (elm.querySelector("#connectionType").value == "mix" && elm.querySelector("#MeshIP").value == "") {
        err.innerHTML = "Provide the DTC Node's IP.";
        return null;
    }
    if (elm.querySelector("#connectionType").value == "mix" && elm.querySelector("#ShfID").value == "") {
        err.innerHTML = "Shahaf Node's Platform ID must be defined.";
        return null;
    }
    if (elm.querySelector("#connectionType").value == "dtc" && elm.querySelector("#MeshPW").value == "") {
        err.innerHTML = "Provide the DTC Node's Password.";
        return null;
    }
    if (elm.querySelector("#connectionType").value == "mix" && elm.querySelector("#MeshPW").value == "") {
        err.innerHTML = "Provide the DTC Node's Password.";
        return;
    }
    if (elm.querySelector("#hasExtendedMesh").value == "yes" && elm.querySelector("#eMeshIP").value == "") {
        err.innerHTML = "Provide Extension Node's IP.";
        return null;
    }
    if (elm.querySelector("#hasExtendedMesh").value == "yes" && elm.querySelector("#eMeshPW").value == "") {
        err.innerHTML = "Provide Extension Node's Password.";
        return null;
    }
    err.innerHTML = "OK!";
    return FinalizeThisNode(elm);
}
function FinalizeThisNode(poll) {
    var name = poll.children[1].children[1].value;
    var ShfID = Number(poll.children[5].children[1].value);
    var MeshIP = poll.children[6].children[1].value;
    var MeshUsername = poll.children[7].children[1].value;
    var MeshPW = poll.children[8].children[1].value;
    var LinkCheckIP = poll.children[9].children[1].value;
    var HasExtendedMesh = poll.children[10].children[1].value;
    if (HasExtendedMesh == "on") HasExtendedMesh = true;
    else HasExtendedMesh = false;
    var StationType = poll.children[3].children[1].value;
    switch (StationType) {
        case "base":
            StationType = 1;
            break;
        case "remote":
            StationType = 2;
            break;
    }
    var ConnectionType = poll.children[4].children[1].value;
    switch (ConnectionType) {
        case "shf":
            ConnectionType = 1;
            break;
        case "dtc":
            ConnectionType = 2;
            break;
        case "mix":
            ConnectionType = 3;
            break;
    }
    var GraphType = poll.children[2].children[1].value;
    switch (GraphType) {
        case "p2p":
            GraphType = 1;
            break;
        case "mesh":
            GraphType = 2;
            break;
    }
    var node = new AnyNode(name, ShfID, MeshIP, MeshUsername, MeshPW, LinkCheckIP, HasExtendedMesh, StationType, ConnectionType, GraphType);
            return node;

}

/*
public const short STATIONTYPE_BASE = 1;
        public const short STATIONTYPE_REMOTE = 2;
        public const short CONNECTIONTYPE_SHF = 1;
        public const short CONNECTIONTYPE_DTC = 2;
        public const short GRAPHTYPE_P2P = 1;
        public const short GRAPHTYPE_MESH = 2;
        public const short LINKTYPE_SHF = 1;
        public const short LINKTYPE_DTC = 2;
        public const short LINKTYPE_MIXED = 3;
        */

function cancelNodeSetup(elm) {
    elm.remove();
}