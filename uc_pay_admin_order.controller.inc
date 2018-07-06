<?php
class uc_pay_admin_order
{


	/**
	 *
	 *
	 *
	 */
	private $order_id = 0;
	private $user = array();
	private $order = array();
	private $cart_id = 0;

	/**
	 * uc_pay_admin_order constructor.
	 * @param $order_id
	 *
	 */

	function __construct()
	{
		$this->populate();
	}

	/**
	 * @return bool
	 *
	 */
	private function populate()
	{

		global $user;
		$arg3 = arg(3);
		$order = uc_order_load($arg3);
		//force user to be logged
		if ($user->uid == 0) drupal_goto('user', array('query' => drupal_get_destination()));
		$this->user = $user;
		//uc_order_load test arg3 & order exist
		if ($order) {
			$this->order = $order;
			$this->order_id = $arg3;
		} else {
			drupal_set_message(t("unable to load your order"), 'error');
			return false;
		}
		$this->cart_id = uc_cart_get_id();

	}

	/**
	 * @return bool
	 *
	 *
	 *
	 */
	public function fill_cart_from_order()
	{

		dpm('uc_pay_admin_order_init');
exit();
		//products too add (OrderProducts)
		$oProducts = $this->order->products;
		//standard cart product
		$cProduct = array('nid' => 0, 'qty' => 1, 'data' => array());
		uc_cart_empty($this->cart_id);
		$mssg = t("your cart as been empty and refill with the order");
		$_SESSION['uc_cart_order_rebuild'] = true;
		unset($_SESSION['cart_order']);


		/**la solution pour eviter le retour a /cart/ found at function uc_cart_checkout() {
		 *
		 *
		 *   elseif (!empty($_SESSION['uc_cart_order_rebuild'])) {
		 * drupal_set_message(t('Your shopping cart contents have changed. Please review your order and try again.'));
		 * drupal_goto('cart');
		 * }
		 *
		 *
		 * $order->modified < REQUEST_TIME - UC_CART_CHECKOUT_TIMEOUT) {
		 */
		/**
		 * preparation des produits avec atribu et option
		 *
		 * le resultat doit etre :
		 *
		 * $p => array(
		 * 'nid' => 65,
		 * 'qty' => 1,
		 * 'data' => array(),
		 * 'attributes' => array(
		 * 4 => '9',
		 * ),
		 * )
		 * ["data"]=>
		 * array(4) {
		 * ["attributes"]=>
		 * array(1) {
		 * ["lieu"]=>
		 * array(1) {
		 * [10]=> "à distance"
		 * }
		 * }
		 * ["shippable"]=>
		 * string(1) "0"
		 * ["type"]=>
		 * string(10) "formations"
		 * ["module"]=>
		 * string(10) "uc_product"
		 * }
		 *
		 *
		 *
		 */


		if (count($oProducts) > 0) {
			foreach ($oProducts as $oproduct) {
				//transfer the product order to product cart
				$nid = $cProduct['nid'] = $oproduct->nid;
				$cProduct['qty'] = $oproduct->qty;
				$cProduct['data'] = $oproduct->data;
				$cProductAttributes = uc_product_get_attributes($nid);
				$aid = 0;
				$oid = 0;

				dvm($cProductAttributes, '$attributes');


				//if product use an attribute
				if (!empty($cProduct['data']['attributes'])) {
					foreach ($cProduct['data']['attributes'] as $kAttribute => $options) {
						//search id attribute by name attribute
						foreach ($cProductAttributes as $cProductAttribute)
							if ($cProductAttribute->name == $kAttribute) $aid = $cProductAttribute->aid;
						//search id options
						foreach ($options as $kOptions => $option)
							$oid = $kOptions;
						//create attributes
						$cProduct['attributes'] = array(intval($aid) => $oid);
					}
					unset($cProduct['data']['attributes']);
				}

				dvm($cProduct, '$cProduct');

				uc_cart_add_item(
					$cProduct['nid'],
					$cProduct['qty'],
					$cProduct['data'] + module_invoke_all('uc_add_to_cart_data', $cProduct),
					$this->cart_id,
					$mssg,
					FALSE,
					TRUE
				);
			}
//dvm(uc_cart_get_contents($cart_id),'after fill up');
			return TRUE;
		} else {
			drupal_set_message(t("there is no product in your order "), 'warning');
			return FALSE;
		}

	}

	/**
	 *
	 * test if the order
	 * @return mixed : BOOL || error mssg
	 *
	 */
	public function test_order()
	{
		//test if this is the good page
		if (arg(2) == 'pay_admin_order') {
			//
			if ($this->user->uid == $this->order->uid
					|| user_access('view all orders') === true) {
				switch ($this->order->order_status) {
					case 'pay_admin_order':
						return true;
				}
			}
		}
		return FALSE;
	}
}