 <?php 
 
    $orderParams =  $params  ["replacements"] ["uc_order"];
    global $base_url;   
 
 ?>
  
 
<p>bonjour <?php  echo $orderParams->delivery_first_name ?>,<br/>
    Un administrateur du site <?php echo variable_get('uc_store_name'); ?> a créer pour vous la commande n°= <?php  echo $orderParams->order_id?>, vous pouvez la visualiser et la valider en cliquant sur un des liens si dessous<br />
    </p>
    
   <p><a href="<?php  echo $base_url  ?>/user/<?php  echo $orderParams->uid?>/orders/<?php  echo $orderParams->order_id?>"> voir la commande n° <?php  echo $orderParams->order_id?></a> sur votre compte en ligne</p>
   <p><a href="<?php  echo $base_url  ?>/cart/checkout/pay_admin_order/<?php  echo $orderParams->order_id?>"> voir et payer la commande n° <?php  echo $orderParams->order_id?></a> sur notre page de paiment en ligne </p>
   <p>en vous remerciant de votre confiance,<br/>
   L'equipe de <?php echo variable_get('uc_store_name'); ?></p>
   <?php if($orderParams->order_comments):?>
   <p>Commentaire de l'administrateur qui a crée la commande:<br />
   <?php
      
           echo '$orderParams->order_comments'. $orderParams->order_comments['#parameters'][1]->message;

   
   ?>
   <?php endif;?>
   </p>
   
   
 