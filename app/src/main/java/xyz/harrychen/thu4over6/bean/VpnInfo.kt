package xyz.harrychen.thu4over6.bean

class VpnInfo {
    var ipv4: String = ""
    var ipv6: String = ""
    var route: String = ""
    var dns1: String = ""
    var dns2: String = ""
    var dns3: String = ""
    var searchDomain: String = ""
    var socketFd: Int = -1

    override fun toString(): String {
        return """
            Remote Server: [$ipv6]
            Tunnel IP: $ipv4
            Route: $route
            DNS Servers: $dns1, $dns2, $dns3
            Search Domain: $searchDomain
        """.trimIndent()
    }
}