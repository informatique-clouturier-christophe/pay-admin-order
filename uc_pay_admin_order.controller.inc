<?php

/**
 * Class uc_pay_admin_order
 *
 * i use a class because it s more simply for autoload
 */
class uc_pay_admin_order
{
	/**
	 * @param $order_id
	 * @return bool
	 *
	 * 	fill the cart and use the order for checkout
	 *  https://test.cmi-menuiserie.fr/cart/checkout/pay_admin_order/168
	 */
	public static function fill_cart_from_order($order_id)
	{
		global $user;
		$order = uc_order_load($order_id);
		//some security check
		if ($order &&
			$user->uid == $order->uid  &&
			$order->order_status == 'pay_admin_order'
		) {

			$cart_id = uc_cart_get_id();
			$mssg = t("your basket will be emptied and filled with the order");

			$oProducts = $order->products;//order products
			$cProduct = array('nid' => 0, 'qty' => 1, 'data' => array());//cart product
			// empty the cart before refill
			uc_cart_empty($cart_id);
			/**
			 *
			 * put somme items from order in cart for pass the uc_cart_checkout test
			 * i have wrote this before found the good solution also it's a complete system for load
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

					//if product use an attribute
					if (!empty($cProduct['data']['attributes'])) {
						//add attributes in standard cart product if not exist
						if (!isset($cProduct['attributes']))
							//prepare attributes
							$cProduct['attributes'] = array();
						//scan data attributes
						foreach ($cProduct['data']['attributes'] as $kAttribute => $options) {
							//search id attribute by name attribute
							foreach ($cProductAttributes as $cProductAttribute) {
								if ($cProductAttribute->name == $kAttribute) {
									//create enter for this attribute
									$aid = intval($cProductAttribute->aid);
									$cProduct['attributes'][$aid] = 'options';
									//this is finish for this attribute
									break;
								}
							}
							// Multiple options for this attribute implies checkbox
							// attribute, which we must store as an array
							if (count($options) > 1) {
								//prepare for multiple option in array
								$cProduct['attributes'][$aid] = array();
								foreach ($options as $kOptions => $option) {
									$oid = intval($kOptions);
									//push option
									$cProduct['attributes'][$aid][$oid] = $kOptions;
								}
							} //if there is only one option like radio box
							else {
								$oid = array_keys($options)[0];
								$cProduct['attributes'][$aid] = $oid;
							}

						}
						//purge data attributes
						unset($cProduct['data']['attributes']);
					}

					uc_cart_add_item(
						$cProduct['nid'],
						$cProduct['qty'],
						$cProduct['data'] + module_invoke_all('uc_add_to_cart_data', $cProduct),
						$cart_id,
						$mssg,
						FALSE,
						TRUE
					);
				}
			}
			/**
			 *
			 * update admin order and fill the cart_order with this id
			 *
			 */
			$order->order_status = 'in_checkout';
			$order->modified = REQUEST_TIME;
			uc_order_save($order);
			$_SESSION['cart_order'] = $order_id;
			//flag for empty cart if the user leave the checkout system
			$_SESSION['uc_pay_admin_order_cart_loaded'] = true;
			//
			unset($_SESSION['uc_cart_order_rebuild']);
			return true;
		}
		else{
			drupal_set_message(t("unable to load your order"), 'error');
			return false;

		}
	}

	/**
	 *
	 *
	 *
	 */
	public static function empty_cart(){

		//update the order
		$order = uc_order_load($_SESSION['cart_order']);
		//preserve the order status if the order is not completed
		if($order->order_status == 'in_checkout')
			$order->order_status = 'pay_admin_order';
		$order->modified = REQUEST_TIME ;
		uc_order_save($order);
		//reset the cart
		unset($_SESSION['cart_order']);
		unset($_SESSION['uc_pay_admin_order_cart_loaded']);
		$_SESSION['uc_cart_order_rebuild'] = true;
		uc_cart_empty(uc_cart_get_id() );
		return true;

	}
}