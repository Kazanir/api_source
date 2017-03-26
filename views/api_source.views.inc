<?php

/**
 * Implements hook_views_data_alter().
 */
function api_source_views_data_alter(&$data) {
  $ad = &$data['api_documentation'];

  $ad['did']['field'] = array(
    'handler' => 'views_handler_field',
  );

  $ad['views_megarow'] = array(
    'title' => t('Megarow links'),
    'help' => t('Outputs links that launch the megarow.'),
    'field' => array(
      'handler' => 'api_source_handler_field_megarow_links',
    ),
  );
}

function api_source_views_default_views_alter(&$views) {
  $view = &$views['api_members'];
  $handler = $view->display['default']->handler;
  $handler->display->display_options['style_options']['columns'] = array(
    'views_megarow' => 'views_megarow',
    'title' => 'title',
    'deprecated' => 'deprecated',
    'modifiers' => 'modifiers',
    'object_type' => 'object_type',
    'summary' => 'summary',
    'title_1' => 'title_1',
    'member_alias' => 'member_alias',
    'reference_count' => 'reference_count',
  );
  $handler->display->display_options['style_plugin'] = 'table_megarows';
  /* Field: API documentation: Documentation ID */
  $new_fields['did']['id'] = 'did';
  $new_fields['did']['table'] = 'api_documentation';
  $new_fields['did']['field'] = 'did';
  $new_fields['did']['relationship'] = 'did';
  $new_fields['did']['label'] = '';
  $new_fields['did']['exclude'] = TRUE;
  $new_fields['did']['alter']['alter_text'] = TRUE;
  $new_fields['did']['element_label_colon'] = FALSE;
  /* Field: API documentation: Object type */
  $new_fields['object_type_1']['id'] = 'object_type_1';
  $new_fields['object_type_1']['table'] = 'api_documentation';
  $new_fields['object_type_1']['field'] = 'object_type';
  $new_fields['object_type_1']['relationship'] = 'did';
  $new_fields['object_type_1']['label'] = '';
  $new_fields['object_type_1']['exclude'] = TRUE;
  $new_fields['object_type_1']['element_label_colon'] = FALSE;
  /* Field: API documentation: Megarow links */
  $new_fields['views_megarow']['id'] = 'views_megarow';
  $new_fields['views_megarow']['table'] = 'api_documentation';
  $new_fields['views_megarow']['field'] = 'views_megarow';
  $new_fields['views_megarow']['relationship'] = 'did';
  $new_fields['views_megarow']['label'] = 'Source';
  $new_fields['views_megarow']['megarow'] = array(
    'links' => 'Code|api/source/[did]/[object_type_1]',
  );

  $handler->display->display_options['fields'] = $new_fields + $handler->display->display_options['fields'];
}

/**
 * Implements hook_views_post_render().
 *
 * Used to add an ID to each of our API member rows in the table view, so that
 * matching members can easily link to them when clicked.
 */
function api_source_views_post_render(&$view, &$output, $cache) {
  if ($view->name == 'api_members') {
    $output = preg_replace_callback('~data-entity-id=\"(\d+)\">~i', function ($matches) {
      $return = 'data-entity-id="' . $matches[1] . '" id="member-' . $matches[1] . '">';
      return $return;
    }, $output);
  }
}

/**
 * Implements hook_module_implements_alter().
 *
 * Because clearly a 200-line module cannot live without it.
 */
function api_source_module_implements_alter(&$implementations, $hook) {
  if ($hook == 'views_post_render') {
    $us = $implementations['api_source'];
    unset($implementations['api_source']);
    $implementations['api_source'] = $us;
  }
}

// DEPRECATED BELOW THIS LINE, NOTES FROM PREVIOUS MAZE PROFILE:

function _api_source_views_pre_render(&$view) {
  if ($view->name == 'api_members' && $view->current_display == 'code_list') {
    foreach ($view->result as $res) {
      $res->api_documentation_api_members_modifiers = str_pad($res->api_documentation_api_members_modifiers, 18);
      $split = explode("::", $res->api_documentation_api_members_title);
      $cn = str_pad($split[0], 35);
      $res->api_documentation_api_members_title = $cn . $split[1];
    }
  }
}
function _api_source_views_post_render(&$view, &$output, $cache) {
  if ($view->name == 'api_members') {
    // Get our anchors by matching up the names of each row to the row ID.
    foreach ($view->result as $id => $row) {
      $object_name = explode('::', $row->api_documentation_api_members_title)[1];
      $anchors[$object_name] = $id;
    }

    dpm($anchors);

    $output = preg_replace_callback('%(constant|function|property) member-of-self\">(\w+)</span>%i', function($matches) use ($anchors) {
      $prefix = ($matches[1] == 'property' ? '$' : '');
      $return = $matches[1];
      $return .= ' member-of-self"><a href="#member-row-' . $anchors[$prefix . $matches[2]];
      // $return .= '" class="accordion-toggle" data-toggle="collapse" data-target="#collapse-1-';
      // $return .= (isset($anchors[$prefix . $matches[2]]) ? $anchors[$prefix . $matches[2]] : $anchors[$matches[2]]);
      $return .= '">' . $matches[2] . '</a></span>';

      return $return;
    }, $output);
    //
  }
}

function _api_source_preprocess_megarow_tables_plugin_style(&$vars) {
  $view = &$vars['view'];
  $title_field = $vars['options']['title_field'];

  if ($view->name == 'api_members' && $view->current_display == 'code_list') {
    foreach (array_keys($vars['rows']) as $key) {
      $pieces = explode(' ', strip_tags($view->style_plugin->get_field($key, $title_field)));
      $last_word = array_pop($pieces);
      $vars['anchors'][$key] = $last_word;
    }
  }

  $view->style_plugin->saved_anchors = $vars['anchors'];
}

