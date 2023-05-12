#include <stdio.h>
#include <unistd.h>

#include "../proxy_testing/client_utils.h"
#include "../http.h"
#include "../more_file.h"
#include "../tcp.h"

void make_request(int client, char *request) {
    checked_write(client, request, strlen(request));
    HTTP_Response_T client_response = block_until_response(client);
    int client1_fd1 = checked_open_for_writing("out_files/proxy_response1");
    write(client1_fd1, client_response->body, client_response->content_len);
    close(client1_fd1);
    HTTP_Response_free(&client_response);
}

int main() {
    int client = connect_to_server("localhost", 9086);
    make_request(
        client,
        "GET /comp/112/index.html HTTP/1.1\r\nHost: www.cs.tufts.edu\r\n\r\n");
    make_request(client,
                 "GET /~prs/bio.html HTTP/1.1\r\nHost: www.cs.cmu.edu\r\n\r\n");
    make_request(
        client,
        "GET /index.html HTTP/1.1\r\nHost: www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ac.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_al.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_an.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_au.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_be.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_bl.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_bu.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ch.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_cm.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_co.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_cp.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_cs.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_cv.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_dc.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_dl.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_dp.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_dr.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_eb.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ed.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_et.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ex.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_fb.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_fi.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_fk.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_gb.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_gm.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_gr.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_gv.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_hk.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_hl.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_hm.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_hr.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_hs.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_hw.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ia.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_il.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_im.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_in.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_io.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ir.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_is.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_it.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_js.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mb.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_md.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mg.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mk.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mn.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mo.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mp.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mr.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ms.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_mv.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_nd.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_np.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ns.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_nt.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_nw.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_os.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pa.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pc.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pd.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ph.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pm.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pn.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_po.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pp.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pr.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ps.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_pu.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ra.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_rb.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_re.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_rh.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_rl.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_rm.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_rs.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_sc.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_se.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_sm.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_sn.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_sp.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_st.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_su.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_sw.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_sx.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_sy.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_tb.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_tf.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_tg.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_ti.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_to.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_tr.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_tt.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_uk.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_uncat.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_vi.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_vs.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_wa.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_we.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_wm.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_wp.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testcat_wx.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testrep_gray.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testrep_red.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testreputation_highrisk.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    make_request(client,
                 "GET /testrep_yellow.html HTTP/1.1\r\nHost: "
                 "www.testingmcafeesites.com\r\n\r\n");
    close(client);
    return 0;
}