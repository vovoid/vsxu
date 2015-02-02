if (cmd == "module_operation_perform")
{
  if (c->parts.size() == 3)
  {
    vsx_comp* comp = get_component_by_name(c->parts[1]);
    vsx_module_operation operation;
    operation.unserialize( c->parts[2] );
    comp->module_operation_run( operation );
  }
}
