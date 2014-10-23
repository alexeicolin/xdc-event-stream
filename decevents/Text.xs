/* 
 *  Copyright (c) 2008 Texas Instruments. All rights reserved. 
 *  This program and the accompanying materials are made available under the 
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at 
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * */
/*
 *  ======== Text.xs ========
 */

var cordMap = {};
var nameMap = {};

var maxDepth = 1;

/*
 *  ======== module$use ========
 */
function module$use()
{
    /* We decide if we should load Registry based on the runtime support 
     * package. If that package is built with the newer version of
     * 'xdc.runtime', Registry code is available in the runtime support package
     * and we load Registry.
     */
    var rts = Program.build.target.rts;
    if ("rtsName" in Program.build) {
        rts = Program.build.rtsName;
    }
    if (rts) {
        var rtsPkg = xdc.loadPackage(rts, true);
        var xml = null;
        var xmlFile = xdc.findFile(rts.replace(".", "/")
                                   + "/package/package.rel.xml");
        if (xmlFile) {
            xml = xdc.loadXML(xmlFile);
        }
        else {
            xmlFile = xdc.findFile(rts.replace(".", "/") + "/package/rel/"
                + rts.replace(".", "_") + "/" + rts.replace(".", "/")
                + "/package/package.rel.xml");
            if (xmlFile) {
                xml = xdc.loadXML(xmlFile);
            }
        } 
        if (xml == null) {
            xdc.useModule('xdc.runtime.Registry');
            return;
        }
        for (var i in xml.references["package"]) {
            if (xml.references["package"][i].@name == "xdc.runtime") {
                var vers = xml.references["package"][i].@version;
                if (vers >= "2, 1, 0") {
                    xdc.useModule('xdc.runtime.Registry');
                }
            }
        }
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(obj)
{
    obj.charBase = this.charTab.$addrof(0);
    obj.nodeBase = this.nodeTab.$addrof(0);
    
    this.genModNames();

    if (!this.isLoaded) {
        var Mem = xdc.module('xdc.runtime.Memory');
        Mem.staticPlace(this.charTab, 0, 'xdc.noload');
        Mem.staticPlace(this.nodeTab, 0, 'xdc.noload');
    }
}

/*
 *  ======== module$view$init ========
 */
function module$view$init(view, obj)
{
    view.charBase = obj.charBase;
    view.charBase = obj.nodeBase;

    var cfg = Program.getModuleConfig('xdc.runtime.Text');

    this.$private.charBase = obj.charBase;
    this.$private.charLast = $addr(obj.charBase + cfg.charCnt);
    this.$private.nodeBase = obj.nodeBase;
    this.$private.nodeLast = $addr(obj.nodeBase
        + (cfg.nodeCnt * this.Node.$sizeof()));
}

/*
 *  ======== defineRopeCord ========
 *  Put text in charTab[] and put its offset (within charTab) into
 *  cordMap[text].
 */
function defineRopeCord(text)
{
    if (!text) {
        return (0);
    }

    var cid;

    if ((cid = cordMap[text]) === undefined) {
        cid = cordMap[text] = this.charTab.length;
        for (var i = 0; i < text.length; i++) {
            this.charTab.$add(text.charCodeAt(i));
        }
        this.charTab.$add(0);
        this.charCnt = this.charTab.length;
    }

    return (cid);
}

/*
 *  ======== defineRopeNode ========
 *  Create a new node and return its node id
 *
 *  Node ids are defined as (index into nodeTab[] | 0x8000). The upper bit
 *  (0x8000) is set to distinguish node ids from offsets into charTab; a
 *  rope id is either a node id or an offset into charTab.  The upper bit
 *  also allows us to use these IDs as module ids; unnamed modules are
 *  simply numbered from 1 to 0x7fff whereas named modules have ids equal
 *  to their "rope node id".
 */
function defineRopeNode(left, right)
{
    var nid = this.nodeTab.length | 0x8000;
    this.nodeTab.$add({left: left, right: right});
    this.nodeCnt++;

    return (nid);
}

/*
 *  ======== fetchAddr ========
 */
function fetchAddr(raddr)
{
    var priv = this.$private;
    
    if (raddr >= priv.charBase && raddr < priv.charLast) {
        return (this.fetchCord(raddr - priv.charBase));
    }
    else if (raddr >= priv.nodeBase && raddr < priv.nodeLast) {
        return (this.fetchNode((raddr - priv.nodeBase) / this.Node.$sizeof()));
    }
    else {
        return (Program.fetchString(raddr, true));
    }
}

/*
 *  ======== fetchCord ========
 */
function fetchCord(cid)
{
    var cfg = Program.getModuleConfig('xdc.runtime.Text');
    var res = "";

    for (var i = cid; cfg.charTab[i]; res += String.fromCharCode(cfg.charTab[i++])) ;

    return (res);
}

/*
 *  ======== fetchId ========
 */
function fetchId(rid)
{
    var cfg = Program.getModuleConfig('xdc.runtime.Text');
    return (
        (rid & 0x8000) ? this.fetchNode(rid & ~0x8000) : this.fetchCord(rid)
    );
}

/*
 *  ======== fetchNode ========
 */
function fetchNode(nid)
{
    var cfg = Program.getModuleConfig('xdc.runtime.Text');
    var node = cfg.nodeTab[nid];

    return (this.fetchId(node.left) + this.fetchId(node.right));
}

/*
 *  ======== genModNames ========
 *  This function generates module IDs by "registering" module names in the 
 *  nodeTab[] and using the returned node ID or by simply incrementing a 
 *  counter starting from one. Node IDs are guarenteed to be 0x8000
 *  or above, so named modules occupy 0x8000-0xFFFF. The range from 0x0001 to
 *  0x7FFF is divided between unnamed modules and Registry modules. 
 *  The module IDs are allocated as follows:
 *
 *     0x0000    - invalid module ID
 *     0x0001    - first unnamed module ID
 *        :
 *     0x4000    - last unnamed module ID
 *     0x4001    - first Registry module ID
 *        :      
 *     0x7FFF    - last Registry module ID
 *     0x8000    - first named module ID
 *        :
 *     0xFFFF    - last named module ID
 *
 *  Named modules have their string names encoded and stored on the target.
 *  Unnamed modules can only be identified by name via the id<->name map
 *  that is generated during configuration (*.rta.xml).
 */
function genModNames()
{
    var idx = 1;
    var Defaults = xdc.module('xdc.runtime.Defaults');
    for each (var mod in Program.targetModules()) {
        var mid;
        if (!mod.PROXY$ && Defaults.getCommon(mod, "namedModule") != false) {
            var k = mod.$name.lastIndexOf('.');
            var d = mod.$package.$name.split('.').length;
            if (d > maxDepth) {
                maxDepth = d;
            }
            mod.Module__id = this.defineRopeNode(
                this.genPkgName(mod.$package.$name),
                this.defineRopeCord(mod.$name.substring(k + 1))
            );
        }
        /* 
         * If this is not a named module, assign it a number starting at 1. 
         * Named modules start at 0x8000.
         */
        else {
            /* Verify we haven't run out of unnamed module ids. */
            if (idx > this.unnamedModsLastId) {
                this.$logFatal("Too many unnamed modules.", this);
            }
            mod.Module__id = idx++;
        }
    }

//    this.unnamedModCnt = idx - 1;
}

/*
 *  ======== genPkgName ========
 */
function genPkgName(qn)
{
    var k = qn.lastIndexOf('.');
    
    if (k == -1) {
        return (this.defineRopeCord(qn + '.'));
    }

    var nid = nameMap[qn];

    if (nid !== undefined) {
        return (nid);
    }

    nid = this.defineRopeNode(
        this.genPkgName(qn.substring(0, k)),
        this.defineRopeCord(qn.substring(k + 1) + '.'));

    nameMap[qn] = nid;

    return (nid);
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,438; 12-19-2013 19:53:38; /db/ztree/library/trees/xdc/xdc-z63x/src/packages/
 */

