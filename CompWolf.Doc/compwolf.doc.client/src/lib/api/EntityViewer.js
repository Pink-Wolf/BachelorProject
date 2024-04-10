import AliasViewer from "./AliasViewer";
import BaseEntityViewer from "./BaseEntityViewer";
import ClassViewer from "./ClassViewer";
import EnumViewer from "./EnumViewer";
import FunctionViewer from "./FunctionViewer";
import VariableViewer from "./VariableViewer";

export default function EntityViewer({ data }) {
	switch (data?.type) {
		case `class`: return <ClassViewer data={data} />
		case `function`: return <FunctionViewer data={data} />
		case `variable`: return <VariableViewer data={data} />
		case `enum`: return <EnumViewer data={data} />
		case `alias`: return <AliasViewer data={data} />
		default: return <BaseEntityViewer />
    }
}