import { getEntity } from "@/lib/api/Entity";
import EntityViewer from "@/lib/api/EntityViewer";

export default async function SubEntityPage({ params }) {
    const owner = await getEntity(params.project, params.header, params.entity)
    const target = params.subentity

    var data = undefined
    if (target == params.entity) {
        data = { ...owner.constructor, type: "function" }
    }
    if (data === undefined) {
        data = owner.dataMembers?.find((i) => {
            return i.name === target
        })
        if (data !== undefined) data = { ...data, type: "variable" }
    }
    if (data === undefined) {
        owner.methodGroups?.find((i) => {
            i.items.find((j) => {
                if (j.name === target) {
                    data = { ...j, type: "function" }
                }

                return (data !== undefined)
            })
            return (data !== undefined)
        })
    }

    data = {
        ...data,
        project: owner.project,
        header: owner.header,
        owner: owner.name,
        namespace: owner.namespace,
        name: target,
    }

    return <EntityViewer data={data} />
}
