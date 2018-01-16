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
	const API_URL = 'https://api.godaddy.com/v1/domains/%s/records/%s/%s'; //Godaddy DNS Api URL | Domain Name - Record Type - Name (Subdomain)
	const TEST_API_URL = 'https://api.ote-godaddy.com/v1/domains/%s/records/%s/%s'; //Incase anyone wants it...
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
		$this->parseHost($host);
		$this->_recordType = ($useIPv6 ? "AAAA" : "A"); //Set record type

		//Check if the needs to be updated
		if($newIp != $this->getDns()["data"]){
			if(!$this->putDns($newIp)){ //Update DNS record
				return true;
			}
			else
			{
				return false;
			}
		}
		return null; //No need to update
	}

	private function getDns()
	{
		$this->setCurlHandle(); //Set curl handler
		$responce = json_decode(curl_exec($this->_curlHandle), true); //Get DNS from godaddy
		if(isset($responce["message"]) || count($responce) < 0){
			//Error Handle
		}
		else if(isset($responce[0])){
			return $responce[0];
		}
	}

	private function putDns($newIp)
	{
		$this->setCurlHandle('PUT'); //Set curl handler
		curl_setopt($this->_curlHandle, CURLOPT_POSTFIELDS, "[{ \"data\":\"{$newIp}\" }]");
		$responce = json_decode(curl_exec($this->_curlHandle), true);

		switch(curl_getinfo($this->_curlHandle, CURLINFO_RESPONSE_CODE)){
			case 200:
				return true;
				break;
			case 400:
			case 403:
			case 401:
			case 404:
			case 422:
			case 429:
			case 500:
			case 504:
			default:
				//Throw Error
				break;
		}
	}

	private function setCurlHandle($method = 'GET')
	{
		if(!is_null($this->_curlHandle)) curl_close($this->_curlHandle); //Close existing connection
		$this->_curlHandle = curl_init($this->getUrl()); //Initialize Curl
		curl_setopt($this->_curlHandle, CURLOPT_CUSTOMREQUEST, $method);
		curl_setopt($this->_curlHandle, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($this->_curlHandle, CURLOPT_HTTPHEADER, //Set Url
		[
			'Content-Type: application/json',
            'Accept: application/json',
            "Authorization: sso-key {$this->_ssoKey}:{$this->_ssoSecret}"
		]);
	}

	private function parseHost($host)
	{
		$parseUrl = explode('.', parse_url(
			strpos($host, '://') ? $host : "http://{$host}"
		)["host"]);

        $this->_host = [ 
        	"domain" => implode('.', array_slice($parseUrl, count($parseUrl) - 2, 2)), 
			"name" => (implode('.', array_slice($parseUrl, 0, count($parseUrl) - 2)) ?: null)
		];
	}

	private function getUrl()
	{
		return sprintf(self::API_URL, 
			$this->_host["domain"], //Domain
			$this->_recordType, //Record Type
			($this->_host["name"]?: "@")  //Name
		);
	}
}