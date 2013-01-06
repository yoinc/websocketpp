/*
 * Copyright (c) 2012, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef WEBSOCKETPP_ENDPOINT_IMPL_HPP
#define WEBSOCKETPP_ENDPOINT_IMPL_HPP

namespace websocketpp {

template <typename connection, typename config>
typename endpoint<connection,config>::connection_ptr
endpoint<connection,config>::create_connection() {
    std::cout << "create_connection" << std::endl;
    //scoped_lock_type lock(m_state_lock);
	
	/*if (m_state == STOPPING || m_state == STOPPED) {
		return connection_ptr();
	}*/
	
    // Create a connection on the heap and manage it using a shared pointer
	connection_ptr con(new connection_type(m_is_server,m_user_agent));
    
    connection_weak_ptr w(con);
    

    // Create a weak pointer on the heap using that shared_ptr.
    // Cast that weak pointer to void* and manage it using another shared_ptr
    // connection_hdl hdl(reinterpret_cast<void*>(new connection_weak_ptr(con)));

    // con->set_handle(hdl);

    //
    con->set_handle(w);
	con->set_handler(m_default_handler);
    con->set_open_handler(m_open_handler);
	con->set_termination_handler(
	    lib::bind(
	        &type::remove_connection,
	        this,
	        lib::placeholders::_1
	    )
	);
	transport_type::init(con);
	m_connections.insert(con);
	
	
	//m_alog->at(log::alevel::DEVEL) << "Connection created: count is now: " 
	//                               << m_connections.size() << log::endl;
	
	return con;
}

template <typename connection, typename config>
void endpoint<connection,config>::remove_connection(connection_ptr con) {
    std::cout << "remove_connection. New count: " << m_connections.size()-1
		      << std::endl;
    scoped_lock_type lock(m_state_lock);
		
	// unregister the termination handler
	con->set_termination_handler(termination_handler());
	
	m_connections.erase(con);
}

} // namespace websocketpp

#endif // WEBSOCKETPP_ENDPOINT_IMPL_HPP