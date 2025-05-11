# eastl_formatters.py

import lldb
import re

def __lldb_init_module(debugger, internal_dict):
    # Register all the formatters
    debugger.HandleCommand('type summary add -F eastl_formatters.unique_ptr_summary "eastl::unique_ptr<*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.shared_ptr_summary "eastl::shared_ptr<*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.weak_ptr_summary "eastl::weak_ptr<*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.eastl_string_summary "eastl::basic_string<char,*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.eastl_wstring_summary "eastl::basic_string<wchar_t,*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.pair_summary "eastl::pair<*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.vector_summary "eastl::VectorBase<*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.array_summary "eastl::array<*,*>"')
    debugger.HandleCommand('type summary add -F eastl_formatters.span_summary "eastl::span<*>"')
    
    # Register synthetic providers for more complex containers
    debugger.HandleCommand('type synthetic add -l eastl_formatters.VectorSyntheticProvider -x "eastl::VectorBase<.*>"')
    debugger.HandleCommand('type synthetic add -l eastl_formatters.ArraySyntheticProvider -x "eastl::array<.*,.*>"')
    debugger.HandleCommand('type synthetic add -l eastl_formatters.SpanSyntheticProvider -x "eastl::span<.*>"')
    
    print("EASTL formatters loaded!")

def unique_ptr_summary(valobj, internal_dict):
    if valobj.IsValid():
        # Extract the pointer value
        ptr = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst')
        if ptr.GetValueAsUnsigned() == 0:
            return '(nullptr)'
        else:
            # Try to dereference and get value
            try:
                value = ptr.Dereference()
                return f"({ptr.GetValueAsUnsigned():x}) = {value.GetSummary()}"
            except:
                return f"({ptr.GetValueAsUnsigned():x})"
    return "Invalid"

def shared_ptr_summary(valobj, internal_dict):
    if valobj.IsValid():
        ptr = valobj.GetChildMemberWithName('mpValue')
        if ptr.GetValueAsUnsigned() == 0:
            return '(nullptr)'
        else:
            try:
                value = ptr.Dereference()
                ref_count = valobj.GetChildMemberWithName('mpRefCount').GetChildMemberWithName('mRefCount')
                return f"({ptr.GetValueAsUnsigned():x}) = {value.GetSummary()} [refs={ref_count.GetValueAsUnsigned()}]"
            except:
                return f"({ptr.GetValueAsUnsigned():x})"
    return "Invalid"

def weak_ptr_summary(valobj, internal_dict):
    if valobj.IsValid():
        ref_count = valobj.GetChildMemberWithName('mpRefCount')
        if ref_count.GetValueAsUnsigned() == 0:
            return '(nullptr)'
        else:
            ptr = valobj.GetChildMemberWithName('mpValue')
            try:
                value = ptr.Dereference()
                return f"{value.GetSummary()}"
            except:
                return f"(expired or invalid)"
    return "Invalid"

def eastl_string_summary(valobj, internal_dict):
    if valobj.IsValid():
        # Extract SSO mask and check if using heap or SSO
        sso_field = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('sso').GetChildMemberWithName('mRemainingSizeField').GetChildMemberWithName('mnRemainingSize')
        try:
            # Attempt to get kSSOMask
            sso_mask_type = valobj.GetType().GetTemplateArgumentType(0).GetPointeeType()
            kSSOMask = valobj.GetFrame().EvaluateExpression(f"(({sso_mask_type.GetName()})1) << ((sizeof({sso_mask_type.GetName()}) * 8) - 1)").GetValueAsUnsigned()
            
            using_heap = (sso_field.GetValueAsUnsigned() & kSSOMask) != 0
            
            if using_heap:
                # Using heap
                data_ptr = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('heap').GetChildMemberWithName('mpBegin')
                length = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('heap').GetChildMemberWithName('mnSize')
                return f'"{data_ptr.GetSummary()}" [len={length.GetValueAsUnsigned()}]'
            else:
                # Using SSO
                data = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('sso').GetChildMemberWithName('mData')
                length = sso_field.GetValueAsUnsigned()
                return f'"{data.GetSummary()}" [len={length}]'
        except:
            return f'"<error reading string>"'
    return "Invalid"

def eastl_wstring_summary(valobj, internal_dict):
    # Similar to eastl_string_summary but for wide strings
    if valobj.IsValid():
        sso_field = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('sso').GetChildMemberWithName('mRemainingSizeField').GetChildMemberWithName('mnRemainingSize')
        try:
            sso_mask_type = valobj.GetType().GetTemplateArgumentType(0).GetPointeeType()
            kSSOMask = valobj.GetFrame().EvaluateExpression(f"(({sso_mask_type.GetName()})1) << ((sizeof({sso_mask_type.GetName()}) * 8) - 1)").GetValueAsUnsigned()
            
            using_heap = (sso_field.GetValueAsUnsigned() & kSSOMask) != 0
            
            if using_heap:
                data_ptr = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('heap').GetChildMemberWithName('mpBegin')
                length = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('heap').GetChildMemberWithName('mnSize')
                return f'"{data_ptr.GetSummary()}" [len={length.GetValueAsUnsigned()}]'
            else:
                data = valobj.GetChildMemberWithName('mPair').GetChildMemberWithName('mFirst').GetChildMemberWithName('sso').GetChildMemberWithName('mData')
                length = sso_field.GetValueAsUnsigned()
                return f'"{data.GetSummary()}" [len={length}]'
        except:
            return f'"<error reading wstring>"'
    return "Invalid"

def pair_summary(valobj, internal_dict):
    if valobj.IsValid():
        first = valobj.GetChildMemberWithName('first')
        second = valobj.GetChildMemberWithName('second')
        return f"({first.GetSummary()}, {second.GetSummary()})"
    return "Invalid"

def array_summary(valobj, internal_dict):
    if valobj.IsValid():
        # Get the size from the template parameter
        size_str = re.findall(r'array<[^,]+,\s*(\d+)>', valobj.GetTypeName())
        if size_str:
            size = int(size_str[0])
            if size == 0:
                return '[0] {}'
            
            values = []
            value_ptr = valobj.GetChildMemberWithName('mValue')
            
            # Only include the first few elements in the summary
            max_display = min(6, size)
            for i in range(max_display):
                if i < size:
                    try:
                        element = value_ptr.GetChildAtIndex(i)
                        values.append(f"{element.GetSummary() if element.GetSummary() else str(element.GetValue())}")
                    except:
                        values.append("?")
            
            if size > max_display:
                values.append("...")
                
            return f"[{size}] {{ {', '.join(values)} }}"
    return "Invalid"

def vector_summary(valobj, internal_dict):
    if valobj.IsValid():
        begin = valobj.GetChildMemberWithName('mpBegin')
        end = valobj.GetChildMemberWithName('mpEnd')
        
        if begin.IsValid() and end.IsValid():
            begin_addr = begin.GetValueAsUnsigned()
            end_addr = end.GetValueAsUnsigned()
            
            # Calculate size
            element_type = begin.GetType().GetPointeeType()
            element_size = element_type.GetByteSize()
            size = (end_addr - begin_addr) // element_size
            
            if size == 0:
                return f"[0] {{}}"
            
            values = []
            # Only include the first few elements in the summary
            max_display = min(6, size)
            for i in range(max_display):
                element_addr = begin_addr + (i * element_size)
                element = valobj.GetProcess().ReadPointerFromMemory(element_addr, 0)
                element_obj = begin.CreateValueFromAddress(f"[{i}]", element_addr, element_type)
                values.append(f"{element_obj.GetSummary() if element_obj.GetSummary() else str(element_obj.GetValue())}")
            
            if size > max_display:
                values.append("...")
                
            return f"[{size}] {{ {', '.join(values)} }}"
    return "Invalid"

def span_summary(valobj, internal_dict):
    if valobj.IsValid():
        data_ptr = valobj.GetChildMemberWithName('mpData')
        size = valobj.GetChildMemberWithName('mnSize').GetValueAsUnsigned()
        
        if size == 0:
            return f"[0] {{}}"
        
        values = []
        # Only include the first few elements in the summary
        max_display = min(6, size)
        for i in range(max_display):
            try:
                element = data_ptr.GetChildAtIndex(i)
                values.append(f"{element.GetSummary() if element.GetSummary() else str(element.GetValue())}")
            except:
                values.append("?")
        
        if size > max_display:
            values.append("...")
            
        return f"[{size}] {{ {', '.join(values)} }}"
    return "Invalid"

# Synthetic providers for containers
class VectorSyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.begin = None
        self.size = 0
        self.element_type = None
        self.element_size = 0
        self.update()

    def update(self):
        try:
            self.begin = self.valobj.GetChildMemberWithName('mpBegin')
            end = self.valobj.GetChildMemberWithName('mpEnd')
            begin_addr = self.begin.GetValueAsUnsigned()
            end_addr = end.GetValueAsUnsigned()
            
            self.element_type = self.begin.GetType().GetPointeeType()
            self.element_size = self.element_type.GetByteSize()
            self.size = (end_addr - begin_addr) // self.element_size
        except:
            self.size = 0

    def num_children(self):
        return self.size

    def get_child_index(self, name):
        try:
            return int(name.lstrip('[').rstrip(']'))
        except:
            return -1

    def get_child_at_index(self, index):
        if index < 0 or index >= self.size:
            return None
        
        try:
            begin_addr = self.begin.GetValueAsUnsigned()
            element_addr = begin_addr + (index * self.element_size)
            return self.begin.CreateValueFromAddress(f"[{index}]", element_addr, self.element_type)
        except:
            return None

class ArraySyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.size = 0
        self.update()

    def update(self):
        try:
            # Extract size from the type name
            size_str = re.findall(r'array<[^,]+,\s*(\d+)>', self.valobj.GetTypeName())
            if size_str:
                self.size = int(size_str[0])
            else:
                self.size = 0
        except:
            self.size = 0

    def num_children(self):
        return self.size

    def get_child_index(self, name):
        try:
            return int(name.lstrip('[').rstrip(']'))
        except:
            return -1

    def get_child_at_index(self, index):
        if index < 0 or index >= self.size:
            return None
        
        try:
            value_ptr = self.valobj.GetChildMemberWithName('mValue')
            return value_ptr.GetChildAtIndex(index)
        except:
            return None

class SpanSyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.data_ptr = None
        self.size = 0
        self.update()

    def update(self):
        try:
            self.data_ptr = self.valobj.GetChildMemberWithName('mpData')
            self.size = self.valobj.GetChildMemberWithName('mnSize').GetValueAsUnsigned()
        except:
            self.size = 0

    def num_children(self):
        return self.size

    def get_child_index(self, name):
        try:
            return int(name.lstrip('[').rstrip(']'))
        except:
            return -1

    def get_child_at_index(self, index):
        if index < 0 or index >= self.size:
            return None
        
        try:
            return self.data_ptr.GetChildAtIndex(index)
        except:
            return None