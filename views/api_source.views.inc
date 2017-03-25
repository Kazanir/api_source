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
    'did' => 'did',
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
  $handler->display->display_options['fields']['did']['id'] = 'did';
  $handler->display->display_options['fields']['did']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['did']['field'] = 'did';
  $handler->display->display_options['fields']['did']['relationship'] = 'did';
  $handler->display->display_options['fields']['did']['label'] = '';
  $handler->display->display_options['fields']['did']['exclude'] = TRUE;
  $handler->display->display_options['fields']['did']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['did']['element_label_colon'] = FALSE;
  /* Field: API documentation: Megarow links */
  $handler->display->display_options['fields']['views_megarow']['id'] = 'views_megarow';
  $handler->display->display_options['fields']['views_megarow']['table'] = 'api_documentation';
  $handler->display->display_options['fields']['views_megarow']['field'] = 'views_megarow';
  $handler->display->display_options['fields']['views_megarow']['relationship'] = 'did';
  $handler->display->display_options['fields']['views_megarow']['label'] = 'Source';
  $handler->display->display_options['fields']['views_megarow']['megarow'] = array(
    'links' => '+|api/source/[did]/[object_type]',
  );
}

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
  if ($view->name == 'api_members' && $view->current_display == 'code_list') {
    // Get our anchors from the results of our preprocess function; we need them to find the accordion ID.
    $anchors = array_flip($view->style_plugin->saved_anchors);

    $output = preg_replace_callback('%(function|property) member-of-self\">(\w+)</span>%i', function($matches) use ($anchors) {
      $prefix = ($matches[1] == 'property' ? '$' : '');
      $return = $matches[1];
      $return .= ' member-of-self"><a href="#code--this::';
      $return .= $prefix;
      $return .= $matches[2];
      $return .= '" class="accordion-toggle" data-toggle="collapse" data-target="#collapse-1-';
      $return .= (isset($anchors[$prefix . $matches[2]]) ? $anchors[$prefix . $matches[2]] : $anchors[$matches[2]]);
      $return .= '">' . $matches[2] . '</a></span>';

      return $return;
    }, $output);
    //
  }
}

