package xyz.harrychen.thu4over6.net

import android.net.VpnService
import xyz.harrychen.thu4over6.bean.VpnInfo

class VpnService4Over6(info: VpnInfo): VpnService() {

    private external fun startForwarding(fd: Int)
}