package xyz.harrychen.thu4over6.net

import android.net.VpnService
import android.os.ParcelFileDescriptor
import android.util.Log
import xyz.harrychen.thu4over6.bean.VpnInfo

class VpnService4Over6: VpnService() {

    private lateinit var vpnFd: ParcelFileDescriptor
    private val TAG = "VPNService"

    fun setup(info: VpnInfo): Int {
        val builder = Builder().setMtu(1400)
            .addAddress(info.ipv4, 32)
            .addRoute(info.route, 0)
            .addDnsServer(info.dns1)
            .addSearchDomain(info.searchDomain)
            .setSession("4over6 to ${info.ipv6}")
        if (info.dns2 != "0.0.0.0") builder.addDnsServer(info.dns2)
        if (info.dns3 != "0.0.0.0") builder.addDnsServer(info.dns3)
        vpnFd = builder.establish()
        Log.d(TAG, "VPN TUN established")
        return vpnFd.fd
    }

    fun stop() {
        Log.d(TAG, "VPN TUN closed")
        vpnFd.close()
    }

}