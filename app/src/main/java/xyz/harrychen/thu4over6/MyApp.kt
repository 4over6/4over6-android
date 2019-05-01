package xyz.harrychen.thu4over6

import android.app.Application
import xyz.harrychen.thu4over6.net.VpnService4Over6

class MyApp: Application() {
    companion object {
        var service: VpnService4Over6 = VpnService4Over6()
    }
}