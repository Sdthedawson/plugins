<?php

/**
*
* Copyright (c) <year>, <copyright holder>
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* The views and conclusions contained in the software and documentation are those
* of the authors and should not be interpreted as representing official policies,
* either expressed or implied, of the FreeBSD Project.
*
*/

/**
* Godaddy Dynamic DNS Over API
*
* @link https://api.godaddy.com/v1
* version 1
*
*/
class GodaddyDns
{
	/* Api Variables */
	const API_URL = "https://api.godaddy.com/v1/domains/%s/records/%s/%s"; //Godaddy DNS Api URL | Domain Name - Record Type - Name (Subdomain)
	protected $_ssoKey; //Godaddy Developer Key
	protected $_ssoSecret; //Godaddy Developer Secret
	protected $_host;
	protected $_recordType;

	/* Curl Handler */
	protected $_curlHandle;

	/**
	* Constructor
	*
	* @param string $ssoKey Developer Key
	* @param string $ssoSecret Developer Secret
	* @return void
	*/
	public function __construct($ssoKey = null, $ssoSecret = null){
		if(!is_null($ssoKey) && !is_null($ssoSecret)){
			$this->_ssoKey = $ssoKey;
			$this->_ssoSecret = $ssoSecret;
		}
	}

	/**
	* Update the Godaddy DNS record
	*
	* @param string $host Hostname to udpate
	* @param string $recordType The record type to use
	*	IPV = A, IPV6 = AAAA
	* @return Update Status
	*/
	public function updateDns($host = null, $useIPv6 = false, $newIp)
	{
		$this->_host = $this->parseHost($host);
		$this->_recordType = ($useIPv6 ? "AAAA" : "A"); //Set record type

		$this->setCurlHandle(); //Set curl handler

		$this->getDns();

		// if($newIp != $oldIp)
		// {

		// }

	} 

	private function setCurlHandle($method = CURLOPT_HTTPGET)
	{
		if(is_null($this->_curlHandle)) curl_close($this->_curlHandle); //Close existing connection
		$this->_curlHandle = curl_init(getUrl()); //Initialize Curl
		//curl_setopt($this->_curlHandle, $method, true);
		curl_setopt($this->_curlHandle, CURLOPT_HEADER, //Set Url
		[
			'Content-Type' => 'application/json',
            'Accept' => 'application/json',
            'Authorization' => 'sso-key {$this->_ssoSecret}:{$this->_ssoKey}'
		]);
	}

	private function parseHost()
	{
		$parseUrl = explode('.', parse_url(
			strpos($this->_host, '://') ? $this->_host : "http://{$this->_host}"
		));
        $this->_host = [ 
        	"domain" => implode('.', array_slice($parseUrl, count($parseUrl) - 2, 2)), 
        	"name" => (implode('.', array_slice($parseUrl, 0, count($parseUrl) - 2)) ?: null)
        ];
	}

	private function getUrl()
	{
		return sprintf(API_UR, 
			$this->host["domain"], //Domain
			$this->_recordType, //Record Type
			($this->_host["name"]?: "@")  //Name
		);
	}

	private function getDns()
	{
		$result = curl_exec($this->_curlHandle);
		echo "";
	}
}

$test = new GodaddyDns("2s83f1JviF_98V5UzArCx4391xokTj5LY", "98V7sFXoGhKE4M8QDh1c4t");
$test->updateDns("test.com", false, "8.8.8.8");
