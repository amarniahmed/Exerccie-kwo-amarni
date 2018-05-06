<?php
        /************************************************************************************************
         * Yahoo.com Contact List Grabber                                                               *
         * Version 1.0                                                                                  *
         * Released 2 July, 2007                                                                      *
         * Author: Ma'moon Al-akash ( soosas@gmail.com )                                                *
         *                                                                                              *
         * This program is free software; you can redistribute it and/or                                *
         * modify it under the terms of the GNU General Public License                                  *
         * as published by the Free Software Foundation; either version 2                               *
         * of the License, or (at your option) any later version.                                       *
         *                                                                                              *
         * This program is distributed in the hope that it will be useful,                              *
         * but WITHOUT ANY WARRANTY; without even the implied warranty of                               *
         * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                *
         * GNU General Public License for more details.                                                 *
         *                                                                                              *
         * You should have received a copy of the GNU General Public License                            *
         * along with this program; if not, write to the Free Software                                  *
         * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.              *
         ************************************************************************************************/

	class yahooGrabber {
		
		var $_username;
		var $_password;
		var $_cookie;
		

		/**
		 * Constructor of the class, initialzing the privates and validates them 
		 * @param $username, yahoo login name
		 * @param $password, yahoo account password
		 */
		function yahooGrabber( $username, $password ) {
			$un	= trim ( $username );
			$pw	= trim ( $password );
			if ( empty ( $un ) )
				die( 'Please Provide your yahoo login name!' );
			if ( empty ( $pw ) )
				die( 'Please Provide your yahoo password account!' );
			$this->_username = $un;
			$this->_password = $pw;
			$this->_cookie	 = '/var/tmp/yahoo_'.$un.'.txt';
		}

		/**
		 * returns $this->_username
		 * @return string
		 */
		function _getUsername() {
			return $this->_username;
		}

		/**
		 * returns $this->_password
		 * @return string
		 */
		function _getPassword() {
			return $this->_password;
		}

		/**
		 * return $this->_cookie, the cookie file path
		 * @return string
		 */
		function _getCookie() {
			return $this->_cookie;
		}

		/**
		 * returns the grabbed set of contacts from the yahoo account in the form of array['name'] = email
		 * @return array
		 */
		function grabYahoo() {
			$ch = curl_init();

			/******************************** AUTHENTICATION SECTION ********************************/
			curl_setopt( $ch, CURLOPT_URL, 'http://login.yahoo.com/config/login?' );
			curl_setopt( $ch, CURLOPT_POST, 22 );
			$postFields = 'login='.$this->_getUsername().'&passwd='.$this->_getPassword().'&.src=&.tries=5&.bypass=&.partner=&.md5=&.hash=&.intl=us&.tries=1&.challenge=ydKtXwwZarNeRMeAufKa56.oJqaO&.u=dmvmk8p231bpr&.yplus=&.emailCode=&pkg=&stepid=&.ev=&hasMsgr=0&.v=0&.chkP=N&.last=&.done=http://address.mail.yahoo.com/';
			curl_setopt( $ch, CURLOPT_POSTFIELDS, $postFields );
			curl_setopt( $ch, CURLOPT_COOKIEJAR, $this->_getCookie() );
			curl_setopt( $ch, CURLOPT_RETURNTRANSFER, 1 );
			if( strstr( curl_exec( $ch ), 'Invalid ID or password' ) ) {
				return array();
			}
			/****************************************************************************************/

			// Set the User Agent
			curl_setopt( $ch, CURLOPT_USERAGENT, 'YahooSeeker-Testing/v3.9 (compatible; Mozilla 4.0; MSIE 5.5; http://search.yahoo.com/)' );
			// Set the URL that PHP will fetch using cURL
			curl_setopt( $ch, CURLOPT_URL, 'http://address.mail.yahoo.com/' );
			// Set the number of fields to be passed via HTTP POST
			curl_setopt( $ch, CURLOPT_POST, 1 );
			// Set the filename where cookie information will be saved
			curl_setopt( $ch, CURLOPT_COOKIEJAR, $this->_getCookie() );
			// Set the filename where cookie information will be looked up
			curl_setopt( $ch, CURLOPT_COOKIEFILE, $this->_getCookie() );
			// Set the option to set Cookie into HTTP header
			curl_setopt( $ch, CURLOPT_COOKIE, 0 );
			curl_setopt( $ch, CURLOPT_RETURNTRANSFER, 1 );
			curl_setopt( $ch, CURLOPT_FOLLOWLOCATION, 1 );

			$contactsGrid = curl_exec( $ch );
			$getTable = array();
			$contactsPages = array();
                        preg_match_all("/\n\s+var InitialContacts \= ([^\n]+)\n/", $contactsGrid, $tmp);
                        $contacts = array();
                        foreach (json_decode(trim($tmp[1][0], ';')) as $contact) {
                          $contacts[$contact->contactName] = $contact->email;
                        }
			$this->_rmFile( $this->_getCookie() );
			return $contacts;
		}

		/**
		 * Remove the target file
		 * @param $fileName, the full path and name of the target file to be removed
		 * @return VOID, nothing to be returned
		 */
		function _rmFile( $fileName ) {
			@unlink( $fileName );
		}
	}
?>
